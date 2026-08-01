package billiards.viewer;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.time.Duration;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.RejectedExecutionException;
import java.util.concurrent.TimeUnit;

import org.junit.jupiter.api.Test;

final class OperationRegistryTest {
    @Test
    void exclusiveOperationRejectsOverlapAndReleasesOnCompletion() {
        final OperationRegistry registry = new OperationRegistry();
        final OperationRegistry.OperationHandle first =
                registry.openExclusive("first", "vary");

        // abdul 28/07/2026 [verify publication is admitted only while a generation and its application owner are active]
        assertTrue(first.permitsPublication());
        assertThrows(
                RejectedExecutionException.class,
                () -> registry.openExclusive("second", "vary"));
        assertTrue(first.complete());
        assertFalse(first.permitsPublication());
        assertFalse(first.complete());

        final OperationRegistry.OperationHandle second =
                registry.openExclusive("second", "vary");
        assertTrue(second.complete());
        assertEquals(0, registry.activeCount());
    }

    @Test
    void completedGenerationKeepsExclusiveKeyUntilOwnedPoolStops()
            throws Exception {
        final OperationRegistry registry = new OperationRegistry();
        final OperationRegistry.OperationHandle operation =
                registry.openExclusive("first", "vary");
        final ExecutorService executor =
                operation.own(Executors.newSingleThreadExecutor());
        final CountDownLatch started = new CountDownLatch(1);
        final CountDownLatch release = new CountDownLatch(1);
        operation.track(executor.submit(() -> {
            started.countDown();
            try {
                release.await();
            } catch (final InterruptedException exception) {
                Thread.currentThread().interrupt();
            }
        }));

        try {
            // abdul 27/07/2026 [prove a completed UI callback cannot admit an overlapping generation while its child pool is still draining]
            assertTrue(started.await(5, TimeUnit.SECONDS));
            assertTrue(operation.complete());
            assertThrows(
                    RejectedExecutionException.class,
                    () -> registry.openExclusive("overlap", "vary"));
        } finally {
            release.countDown();
            if (!operation.isTerminal()) {
                operation.cancel();
            }
        }

        assertTrue(operation.await(Duration.ofSeconds(5)));
        final OperationRegistry.OperationHandle next =
                registry.openExclusive("next", "vary");
        assertTrue(next.complete());
        assertEquals(0, registry.activeCount());
    }

    @Test
    void shutdownCancelsFutureJoinsExecutorAndStopsAdmission() throws Exception {
        final OperationRegistry registry = new OperationRegistry();
        final OperationRegistry.OperationHandle operation =
                registry.open("blocking");
        final ExecutorService executor =
                operation.own(Executors.newSingleThreadExecutor());
        final CountDownLatch started = new CountDownLatch(1);
        operation.track(executor.submit(() -> {
            started.countDown();
            try {
                Thread.sleep(TimeUnit.MINUTES.toMillis(5));
            } catch (final InterruptedException exception) {
                Thread.currentThread().interrupt();
            }
        }));
        assertTrue(started.await(5, TimeUnit.SECONDS));

        // abdul 27/07/2026 [assert the shutdown result instead of accepting a timed-out registry as a passing lifecycle test]
        assertTrue(registry.shutdownAsync(Duration.ofSeconds(5))
                .get(10, TimeUnit.SECONDS));

        assertTrue(operation.isCancelled());
        assertFalse(operation.permitsPublication());
        assertTrue(executor.isTerminated());
        assertEquals(0, registry.activeCount());
        assertThrows(
                RejectedExecutionException.class,
                () -> registry.open("late"));
    }
}
