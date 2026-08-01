package billiards.viewer;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.AtomicMoveNotSupportedException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.HexFormat;
import java.util.List;
import java.util.UUID;
import java.util.concurrent.Callable;
import java.util.concurrent.locks.ReentrantLock;

/**
 * Serialized, staged replacement boundary for the live native cover artifact.
 */
final class CoverArtifactService {
    private static final ReentrantLock ARTIFACT_LOCK =
            new ReentrantLock(true);
    private static final List<String> REQUIRED_FILES = List.of(
            "info.txt", "polygon.txt", "square.txt", "stables.txt",
            "triples.txt", "cover.txt", "precision.txt");

    @FunctionalInterface
    interface MergeAction {
        void merge(Path stagingDirectory) throws Exception;
    }

    record MergeResult(Path liveDirectory, Path rollbackDirectory) {
    }

    private CoverArtifactService() {
    }

    static MergeResult replaceMergedCover(
            final Path requestedLiveDirectory,
            final MergeAction mergeAction) {
        final Path liveDirectory =
                requestedLiveDirectory.toAbsolutePath().normalize();
        final Path parent = liveDirectory.getParent();
        if (parent == null) {
            throw new IllegalArgumentException(
                    "The live cover must have a parent directory.");
        }

        ARTIFACT_LOCK.lock();
        Path stagingDirectory = null;
        Path rollbackDirectory = null;
        boolean liveMoved = false;
        try {
            Files.createDirectories(parent);
            stagingDirectory = parent.resolve(
                    liveDirectory.getFileName() + ".merge-"
                            + UUID.randomUUID());
            Files.createDirectory(stagingDirectory);
            mergeAction.merge(stagingDirectory);
            validateAndWriteManifest(stagingDirectory);

            if (Files.exists(liveDirectory)) {
                rollbackDirectory = parent.resolve(
                        liveDirectory.getFileName() + ".rollback-"
                                + UUID.randomUUID());
                // abdul 27/07/2026 [retain the last live generation before switching to staged output]
                moveDirectory(liveDirectory, rollbackDirectory);
                liveMoved = true;
            }
            moveDirectory(stagingDirectory, liveDirectory);
            stagingDirectory = null;
            return new MergeResult(liveDirectory, rollbackDirectory);
        } catch (final Exception exception) {
            if (liveMoved && !Files.exists(liveDirectory)
                    && rollbackDirectory != null) {
                try {
                    moveDirectory(rollbackDirectory, liveDirectory);
                    rollbackDirectory = null;
                } catch (final IOException rollbackFailure) {
                    exception.addSuppressed(rollbackFailure);
                }
            }
            throw new RuntimeException(
                    "Staged cover merge failed; the prior live generation was preserved.",
                    exception);
        } finally {
            if (stagingDirectory != null) {
                try {
                    deleteTree(stagingDirectory);
                } catch (final IOException cleanupFailure) {
                    System.err.println(
                            "Unable to remove failed cover staging directory: "
                                    + cleanupFailure.getMessage());
                }
            }
            ARTIFACT_LOCK.unlock();
        }
    }

    static void withArtifactLock(final Runnable action) {
        ARTIFACT_LOCK.lock();
        try {
            action.run();
        } finally {
            ARTIFACT_LOCK.unlock();
        }
    }

    static <T> T callWithArtifactLock(final Callable<T> action) {
        ARTIFACT_LOCK.lock();
        try {
            // abdul 27/07/2026 [serialize value-returning native cover writers with merge and metadata publication]
            return action.call();
        } catch (final RuntimeException exception) {
            throw exception;
        } catch (final Exception exception) {
            throw new RuntimeException(
                    "Cover artifact operation failed.", exception);
        } finally {
            ARTIFACT_LOCK.unlock();
        }
    }

    private static void validateAndWriteManifest(
            final Path stagingDirectory) throws IOException {
        final ArrayList<String> manifestLines = new ArrayList<>();
        for (final String name : REQUIRED_FILES) {
            final Path artifact = stagingDirectory.resolve(name);
            if (!Files.isRegularFile(artifact)
                    || Files.isSymbolicLink(artifact)) {
                throw new IOException(
                        "Merged cover is missing regular file " + name);
            }
            manifestLines.add(
                    sha256(artifact) + " " + Files.size(artifact)
                            + " " + name);
        }
        // abdul 27/07/2026 [bind every required merged file to a durable size and SHA-256 manifest]
        Files.write(
                stagingDirectory.resolve("manifest.sha256"),
                manifestLines, StandardCharsets.UTF_8);
    }

    private static String sha256(final Path file) throws IOException {
        try {
            final MessageDigest digest =
                    MessageDigest.getInstance("SHA-256");
            try (var input = Files.newInputStream(file)) {
                final byte[] buffer = new byte[64 * 1024];
                int read;
                while ((read = input.read(buffer)) >= 0) {
                    digest.update(buffer, 0, read);
                }
            }
            return HexFormat.of().formatHex(digest.digest());
        } catch (final NoSuchAlgorithmException exception) {
            throw new AssertionError("SHA-256 is required by the JRE.",
                    exception);
        }
    }

    private static void moveDirectory(
            final Path source, final Path target) throws IOException {
        try {
            Files.move(source, target,
                    StandardCopyOption.ATOMIC_MOVE);
        } catch (final AtomicMoveNotSupportedException exception) {
            Files.move(source, target);
        }
    }

    private static void deleteTree(final Path root) throws IOException {
        if (!Files.exists(root)) {
            return;
        }
        // abdul 27/07/2026 [cleanup is confined to the unique failed staging generation]
        try (var paths = Files.walk(root)) {
            final List<Path> ordered = paths.sorted(
                    java.util.Comparator.reverseOrder()).toList();
            for (final Path path : ordered) {
                Files.deleteIfExists(path);
            }
        }
    }
}
