package billiards.viewer;

import java.io.BufferedWriter;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.AtomicMoveNotSupportedException;
import java.nio.file.Files;
import java.nio.file.InvalidPathException;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;
import java.nio.file.StandardOpenOption;
import java.util.List;

final class SaveV3FileService {

    private SaveV3FileService() {
    }

    // abdul 27/07/2026 [convert the editable destination into one normalized validated path used by save and clear]
    static Path parseDestination(final String destinationText) {
        if (destinationText == null || destinationText.isBlank()) {
            throw new IllegalArgumentException("Select or enter a destination text file.");
        }

        final Path destination;
        try {
            destination = Path.of(destinationText.trim()).toAbsolutePath().normalize();
        } catch (final InvalidPathException exception) {
            throw new IllegalArgumentException("The destination path is invalid.", exception);
        }

        if (!destination.toString().endsWith(".txt")) {
            throw new IllegalArgumentException("The destination must end with .txt.");
        }
        if (destination.getParent() == null || !Files.isDirectory(destination.getParent())) {
            throw new IllegalArgumentException("The destination directory does not exist.");
        }
        if (Files.exists(destination) && !Files.isRegularFile(destination)) {
            throw new IllegalArgumentException("The destination is not a regular file.");
        }
        return destination;
    }

    // abdul 27/07/2026 [validate the complete prefix request before opening or creating its destination]
    static int parseCount(final String countText, final int availableCodes) {
        final int requested;
        try {
            requested = Integer.parseInt(countText == null ? "" : countText.trim());
        } catch (final NumberFormatException exception) {
            throw new IllegalArgumentException("Count must be a whole number.", exception);
        }

        if (requested < 1) {
            throw new IllegalArgumentException("Count must be at least 1.");
        }
        if (requested > availableCodes) {
            throw new IllegalArgumentException(
                "Count is greater than the " + availableCodes + " available code sequences.");
        }
        return requested;
    }

    // abdul 27/07/2026 [stage UTF-8 append output beside the destination and replace only after every requested line is written]
    static void appendPrefixAtomically(
            final Path destination,
            final List<String> codes,
            final int requestedCount) throws IOException {
        if (requestedCount < 1 || requestedCount > codes.size()) {
            throw new IllegalArgumentException("Requested prefix is outside the available code range.");
        }

        final Path temporary = createTemporarySibling(destination);
        boolean replaced = false;
        try {
            if (Files.exists(destination)) {
                Files.copy(
                    destination,
                    temporary,
                    StandardCopyOption.REPLACE_EXISTING,
                    StandardCopyOption.COPY_ATTRIBUTES);
            }

            try (BufferedWriter writer = Files.newBufferedWriter(
                    temporary,
                    StandardCharsets.UTF_8,
                    StandardOpenOption.APPEND)) {
                for (int index = 0; index < requestedCount; ++index) {
                    writer.write(codes.get(index));
                    writer.write('\n');
                }
            }

            replaceDestination(temporary, destination);
            replaced = true;
        } finally {
            if (!replaced) {
                Files.deleteIfExists(temporary);
            }
        }
    }

    // abdul 27/07/2026 [stage an empty sibling before replacing the selected file so clear never truncates an earlier path]
    static void clearAtomically(final Path destination) throws IOException {
        final Path temporary = createTemporarySibling(destination);
        boolean replaced = false;
        try {
            replaceDestination(temporary, destination);
            replaced = true;
        } finally {
            if (!replaced) {
                Files.deleteIfExists(temporary);
            }
        }
    }

    // abdul 27/07/2026 [keep staging on the destination filesystem so an atomic move is possible]
    private static Path createTemporarySibling(final Path destination) throws IOException {
        return Files.createTempFile(
            destination.getParent(),
            "." + destination.getFileName().toString() + "-",
            ".tmp");
    }

    // abdul 27/07/2026 [prefer atomic replacement and retain a defined same-directory fallback for filesystems without it]
    private static void replaceDestination(final Path temporary, final Path destination) throws IOException {
        try {
            Files.move(
                temporary,
                destination,
                StandardCopyOption.ATOMIC_MOVE,
                StandardCopyOption.REPLACE_EXISTING);
        } catch (final AtomicMoveNotSupportedException exception) {
            Files.move(temporary, destination, StandardCopyOption.REPLACE_EXISTING);
        }
    }
}
