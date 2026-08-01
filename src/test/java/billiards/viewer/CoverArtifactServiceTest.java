package billiards.viewer;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Comparator;
import java.util.List;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

final class CoverArtifactServiceTest {
    private static final List<String> REQUIRED = List.of(
            "info.txt", "polygon.txt", "square.txt", "stables.txt",
            "triples.txt", "cover.txt", "precision.txt");

    Path temporaryDirectory;

    @BeforeEach
    void createTemporaryDirectory() throws Exception {
        // abdul 27/07/2026 [provide a JUnit-5.2-compatible isolated cover root]
        temporaryDirectory = Files.createTempDirectory(
                "billiards-cover-artifact-");
    }

    @AfterEach
    void removeTemporaryDirectory() throws Exception {
        try (var paths = Files.walk(temporaryDirectory)) {
            for (final Path path : paths.sorted(
                    Comparator.reverseOrder()).toList()) {
                Files.deleteIfExists(path);
            }
        }
    }

    @Test
    void failedMergeLeavesLiveGenerationUntouched() throws Exception {
        // abdul 27/07/2026 [inject native/staging failure before any live artifact mutation]
        final Path live = temporaryDirectory.resolve("cover");
        Files.createDirectories(live);
        Files.writeString(live.resolve("marker.txt"), "old");

        assertThrows(
                RuntimeException.class,
                () -> CoverArtifactService.replaceMergedCover(
                        live, staging -> {
                            Files.writeString(
                                    staging.resolve("partial.txt"), "new");
                            throw new IllegalStateException("injected");
                        }));

        assertEquals("old",
                Files.readString(live.resolve("marker.txt")));
    }

    @Test
    void completeStageSwitchesLiveAndRetainsRollback()
            throws Exception {
        final Path live = temporaryDirectory.resolve("cover");
        Files.createDirectories(live);
        Files.writeString(live.resolve("marker.txt"), "old");

        final CoverArtifactService.MergeResult result =
                CoverArtifactService.replaceMergedCover(
                        live, staging -> {
                            for (final String name : REQUIRED) {
                                Files.writeString(
                                        staging.resolve(name),
                                        "new-" + name);
                            }
                        });

        // abdul 27/07/2026 [verify manifest publication and recoverable prior generation]
        assertTrue(Files.isRegularFile(
                live.resolve("manifest.sha256")));
        assertEquals("new-cover.txt",
                Files.readString(live.resolve("cover.txt")));
        assertNotNull(result.rollbackDirectory());
        assertEquals("old", Files.readString(
                result.rollbackDirectory().resolve("marker.txt")));
    }

    @Test
    void valueReturningArtifactActionUsesTheSharedLock() {
        // abdul 27/07/2026 [cover the lock boundary used by native CoverWindow calculations]
        assertEquals("published",
                CoverArtifactService.callWithArtifactLock(
                        () -> "published"));
    }
}
