package billiards.viewer;

import billiards.wrapper.Wrapper;

import java.time.Duration;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Future;
import java.util.concurrent.RejectedExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicLong;

/**
 * Application-lifetime ownership for operation-specific tasks and executors.
 */
public final class OperationRegistry {
    // abdul 31/07/2026 [serialize every UI workflow that shares the native Vary cancellation channel]
    public static final String NATIVE_VARY_OPERATION_KEY = "native-vary";
    private final AtomicLong nextGeneration = new AtomicLong();
    private final ConcurrentHashMap<Long, OperationHandle> active =
            new ConcurrentHashMap<>();
    private final ConcurrentHashMap<String, OperationHandle> exclusive =
            new ConcurrentHashMap<>();
    private final AtomicBoolean accepting = new AtomicBoolean(true);
    private final Object admissionLock = new Object();

    public OperationHandle open(final String label) {
        return open(label, null);
    }

    public OperationHandle openExclusive(final String label, final String key) {
        if (key == null || key.isBlank()) {
            throw new IllegalArgumentException("Exclusive operation key is required.");
        }
        final OperationHandle handle = open(label, key);
        if (NATIVE_VARY_OPERATION_KEY.equals(key)) {
            try {
                // abdul 31/07/2026 [reset the native cancel latch only after global Vary exclusivity is owned]
                Wrapper.beginNativeVaryOperation();
            } catch (final RuntimeException exception) {
                handle.cancel();
                throw exception;
            }
        }
        return handle;
    }

    private OperationHandle open(final String label, final String exclusiveKey) {
        synchronized (admissionLock) {
            if (!accepting.get()) {
                throw new RejectedExecutionException(
                        "Application shutdown has stopped new operations.");
            }
            final long generation = nextGeneration.incrementAndGet();
            final OperationHandle handle =
                    new OperationHandle(this, generation, label, exclusiveKey);
            if (exclusiveKey != null
                    && exclusive.putIfAbsent(exclusiveKey, handle) != null) {
                throw new RejectedExecutionException(
                        "Operation is already active: " + exclusiveKey);
            }
            active.put(generation, handle);
            return handle;
        }
    }

    private void release(final OperationHandle handle) {
        active.remove(handle.generation(), handle);
        if (handle.exclusiveKey != null) {
            exclusive.remove(handle.exclusiveKey, handle);
        }
    }

    public boolean isAccepting() {
        return accepting.get();
    }

    public int activeCount() {
        return active.size();
    }

    public Optional<OperationHandle> activeExclusive(final String key) {
        return Optional.ofNullable(exclusive.get(key));
    }

    public CompletableFuture<Boolean> shutdownAsync(final Duration timeout) {
        final List<OperationHandle> snapshot;
        synchronized (admissionLock) {
            accepting.set(false);
            snapshot = new ArrayList<>(active.values());
        }
        snapshot.forEach(OperationHandle::cancel);
        // abdul 27/07/2026 [join all registered operation resources off the JavaFX thread before native-pool destruction and Platform exit]
        return CompletableFuture.supplyAsync(() -> {
            final long deadline =
                    System.nanoTime() + timeout.toNanos();
            boolean stopped = true;
            for (final OperationHandle handle : snapshot) {
                final long remaining = deadline - System.nanoTime();
                if (remaining <= 0) {
                    return false;
                }
                stopped &= handle.await(Duration.ofNanos(remaining));
            }
            return stopped;
        });
    }

    public static final class OperationHandle implements AutoCloseable {
        private final OperationRegistry owner;
        private final long generation;
        private final String label;
        private final String exclusiveKey;
        private final CopyOnWriteArrayList<Future<?>> futures =
                new CopyOnWriteArrayList<>();
        private final CopyOnWriteArrayList<ExecutorService> executors =
                new CopyOnWriteArrayList<>();
        private final AtomicBoolean terminal = new AtomicBoolean(false);
        private final AtomicBoolean cancelled = new AtomicBoolean(false);

        private OperationHandle(
                final OperationRegistry owner,
                final long generation,
                final String label,
                final String exclusiveKey) {
            this.owner = owner;
            this.generation = generation;
            this.label = label;
            this.exclusiveKey = exclusiveKey;
        }

        public long generation() {
            return generation;
        }

        public String label() {
            return label;
        }

        public boolean isCancelled() {
            return cancelled.get();
        }

        public boolean isTerminal() {
            return terminal.get();
        }

        public boolean permitsPublication() {
            // abdul 28/07/2026 [give JavaFX callbacks one atomic policy for rejecting terminal or application-shutdown publication]
            return owner.isAccepting() && !terminal.get();
        }

        public <T extends Future<?>> T track(final T future) {
            if (terminal.get()) {
                future.cancel(true);
                throw new RejectedExecutionException(
                        "Operation is already terminal: " + label);
            }
            futures.add(future);
            if (terminal.get() && futures.remove(future)) {
                future.cancel(true);
            }
            return future;
        }

        public <T extends ExecutorService> T own(final T executor) {
            if (terminal.get()) {
                executor.shutdownNow();
                throw new RejectedExecutionException(
                        "Operation is already terminal: " + label);
            }
            executors.add(executor);
            if (terminal.get() && executors.remove(executor)) {
                executor.shutdownNow();
            }
            return executor;
        }

        public boolean complete() {
            if (!terminal.compareAndSet(false, true)) {
                return false;
            }
            executors.forEach(ExecutorService::shutdown);
            retireWhenStopped();
            return true;
        }

        public boolean cancel() {
            if (!terminal.compareAndSet(false, true)) {
                return false;
            }
            cancelled.set(true);
            if (NATIVE_VARY_OPERATION_KEY.equals(exclusiveKey)) {
                // abdul 31/07/2026 [signal native work before interrupting Java owners so active and queued Vary calls stop together]
                Wrapper.requestVaryCancellation();
            }
            futures.forEach(future -> future.cancel(true));
            executors.forEach(ExecutorService::shutdownNow);
            retireWhenStopped();
            return true;
        }

        private void retireWhenStopped() {
            if (executors.stream().allMatch(
                    ExecutorService::isTerminated)) {
                owner.release(this);
                return;
            }
            // abdul 27/07/2026 [retain terminal generations until owned pools really stop so overlap and shutdown cannot lose them]
            CompletableFuture.runAsync(() -> {
                await(Duration.ofDays(365));
            });
        }

        public boolean await(final Duration timeout) {
            final long deadline =
                    System.nanoTime() + timeout.toNanos();
            boolean stopped = true;
            for (final ExecutorService executor : executors) {
                final long remaining = deadline - System.nanoTime();
                if (remaining <= 0) {
                    return false;
                }
                try {
                    stopped &= executor.awaitTermination(
                            remaining, TimeUnit.NANOSECONDS);
                } catch (final InterruptedException exception) {
                    Thread.currentThread().interrupt();
                    return false;
                }
            }
            if (stopped && terminal.get()) {
                owner.release(this);
            }
            return stopped;
        }

        @Override
        public void close() {
            cancel();
        }
    }
}
