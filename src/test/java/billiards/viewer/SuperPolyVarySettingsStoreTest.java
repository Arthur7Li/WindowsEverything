package billiards.viewer;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Comparator;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

final class SuperPolyVarySettingsStoreTest {
    Path temporaryDirectory;

    @BeforeEach
    void createTemporaryDirectory() throws Exception {
        // abdul 27/07/2026 [provide a JUnit-5.2-compatible isolated settings directory]
        temporaryDirectory = Files.createTempDirectory(
                "billiards-super-settings-");
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
    void completeSettingsRoundTripWithoutLegacySchemaInterference()
            throws Exception {
        // abdul 27/07/2026 [lock the complete versioned Super schedule across restart]
        final Path settingsFile =
                temporaryDirectory.resolve("super.properties");
        final Path legacyBounds =
                temporaryDirectory.resolve("bounds.txt");
        final Path legacySteps =
                temporaryDirectory.resolve("steps.txt");
        Files.writeString(legacyBounds, "1 2 3 4 5 6");
        Files.writeString(legacySteps, "7 8 9");
        final SuperPolyVarySettings expected =
                new SuperPolyVarySettings(
                        10, 11, 12, 13, 14, 15,
                        -2, 3, 4, 5, false, false,
                        true, true, 1.25);

        SuperPolyVarySettingsStore.save(settingsFile, expected);
        Files.writeString(legacyBounds, "90 91 92 93 94 95");
        Files.writeString(legacySteps, "96 97 98");

        assertEquals(expected, SuperPolyVarySettingsStore.load(
                settingsFile, legacyBounds, legacySteps));
    }

    @Test
    void legacyImportReadsMaximaAndSeparateStepFile() throws Exception {
        // abdul 27/07/2026 [regress the prior bug that parsed step tokens from the bounds array]
        final Path settingsFile =
                temporaryDirectory.resolve("missing.properties");
        final Path legacyBounds =
                temporaryDirectory.resolve("bounds.txt");
        final Path legacySteps =
                temporaryDirectory.resolve("steps.txt");
        Files.writeString(legacyBounds, "10 11 12 13 14 15");
        Files.writeString(legacySteps, "21 22 23");

        final SuperPolyVarySettings imported =
                SuperPolyVarySettingsStore.load(
                        settingsFile, legacyBounds, legacySteps);

        assertEquals(10, imported.csMaximum());
        assertEquals(15, imported.osnoSideSumMaximum());
        assertEquals(21, imported.csStep());
        assertEquals(23, imported.osnoStep());
    }
}
