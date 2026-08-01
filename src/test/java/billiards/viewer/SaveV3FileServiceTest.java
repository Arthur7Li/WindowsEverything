package billiards.viewer;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;

import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Comparator;
import java.util.List;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

public final class SaveV3FileServiceTest {

    Path temporaryDirectory;

    @BeforeEach
    public void createTemporaryDirectory() throws Exception {
        // abdul 27/07/2026 [provide a JUnit-5.2-compatible isolated filesystem fixture for export tests]
        temporaryDirectory = Files.createTempDirectory("billiards-save-v3-test-");
    }

    @AfterEach
    public void removeTemporaryDirectory() throws Exception {
        if (temporaryDirectory == null || !Files.exists(temporaryDirectory)) {
            return;
        }
        try (var paths = Files.walk(temporaryDirectory)) {
            paths.sorted(Comparator.reverseOrder()).forEach(path -> {
                try {
                    Files.deleteIfExists(path);
                } catch (final Exception exception) {
                    throw new RuntimeException(exception);
                }
            });
        }
    }

    @Test
    public void typedNewDestinationAppendsOnlyTheValidatedPrefix() throws Exception {
        // abdul 27/07/2026 [prove a trimmed typed path is authoritative and missing files follow the same count contract]
        final Path destination = temporaryDirectory.resolve("typed.txt");
        final Path parsed = SaveV3FileService.parseDestination(
            "  " + destination.toString() + "  ");
        final int count = SaveV3FileService.parseCount(" 2 ", 3);

        SaveV3FileService.appendPrefixAtomically(parsed, List.of("A", "B", "C"), count);

        assertEquals("A\nB\n", Files.readString(destination, StandardCharsets.UTF_8));
    }

    @Test
    public void existingDestinationIsReplacedOnlyAfterACompleteAppend() throws Exception {
        final Path destination = temporaryDirectory.resolve("existing.txt");
        Files.writeString(destination, "old\n", StandardCharsets.UTF_8);

        SaveV3FileService.appendPrefixAtomically(destination, List.of("A", "B"), 1);

        assertEquals("old\nA\n", Files.readString(destination, StandardCharsets.UTF_8));
    }

    @Test
    public void invalidCountsDoNotCreateOrModifyTheDestination() throws Exception {
        // abdul 27/07/2026 [lock the 1-through-size count boundary before any destination mutation]
        final Path missing = temporaryDirectory.resolve("missing.txt");
        assertThrows(
            IllegalArgumentException.class,
            () -> SaveV3FileService.parseCount("not-a-number", 2));
        assertThrows(
            IllegalArgumentException.class,
            () -> SaveV3FileService.parseCount("0", 2));
        assertThrows(
            IllegalArgumentException.class,
            () -> SaveV3FileService.parseCount("-1", 2));
        assertThrows(
            IllegalArgumentException.class,
            () -> SaveV3FileService.parseCount("3", 2));
        assertFalse(Files.exists(missing));

        final Path existing = temporaryDirectory.resolve("unchanged.txt");
        Files.writeString(existing, "keep", StandardCharsets.UTF_8);
        assertThrows(
            IllegalArgumentException.class,
            () -> SaveV3FileService.appendPrefixAtomically(existing, List.of("A"), 2));
        assertEquals("keep", Files.readString(existing, StandardCharsets.UTF_8));
    }

    @Test
    public void clearChangesOnlyTheParsedVisibleDestination() throws Exception {
        // abdul 27/07/2026 [guard the destructive command against silently reusing an earlier browsed/default path]
        final Path earlier = temporaryDirectory.resolve("earlier.txt");
        final Path visible = temporaryDirectory.resolve("visible.txt");
        Files.writeString(earlier, "preserve", StandardCharsets.UTF_8);
        Files.writeString(visible, "clear", StandardCharsets.UTF_8);

        SaveV3FileService.clearAtomically(
            SaveV3FileService.parseDestination(visible.toString()));

        assertEquals("preserve", Files.readString(earlier, StandardCharsets.UTF_8));
        assertEquals("", Files.readString(visible, StandardCharsets.UTF_8));
    }
}
