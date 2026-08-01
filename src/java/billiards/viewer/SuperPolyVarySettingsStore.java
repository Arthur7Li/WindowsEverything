package billiards.viewer;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.AtomicMoveNotSupportedException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;
import java.util.Properties;

/**
 * Versioned, atomic persistence for the complete SuperPolyVary dialog schema.
 */
final class SuperPolyVarySettingsStore {
    private static final String VERSION = "1";

    private SuperPolyVarySettingsStore() {
    }

    static SuperPolyVarySettings load(
            final Path file,
            final Path legacyBounds,
            final Path legacySteps) {
        if (Files.isRegularFile(file)) {
            final Properties properties = new Properties();
            try (InputStream input = Files.newInputStream(file)) {
                properties.load(input);
                if (!VERSION.equals(properties.getProperty("version"))) {
                    throw new IllegalArgumentException(
                            "Unsupported SuperPolyVary settings version.");
                }
                // abdul 27/07/2026 [load every schedule field from the dedicated versioned schema]
                return new SuperPolyVarySettings(
                        integer(properties, "csMaximum"),
                        integer(properties, "osoMaximum"),
                        integer(properties, "osnoMaximum"),
                        integer(properties, "csSideSumMaximum"),
                        integer(properties, "osoSideSumMaximum"),
                        integer(properties, "osnoSideSumMaximum"),
                        integer(properties, "csStep"),
                        integer(properties, "osoStep"),
                        integer(properties, "osnoStep"),
                        integer(properties, "repetitions"),
                        bool(properties, "colorCycle"),
                        bool(properties, "autoCover"),
                        bool(properties, "autoSmallCover"),
                        bool(properties, "magnify"),
                        decimal(properties, "magnification"));
            } catch (final IOException | RuntimeException exception) {
                return SuperPolyVarySettings.defaults();
            }
        }
        return importLegacy(legacyBounds, legacySteps);
    }

    private static SuperPolyVarySettings importLegacy(
            final Path boundsFile, final Path stepsFile) {
        final SuperPolyVarySettings defaults =
                SuperPolyVarySettings.defaults();
        try {
            final String[] bounds = Files.readString(boundsFile).trim()
                    .split("\\s+");
            final String[] steps = Files.readString(stepsFile).trim()
                    .split("\\s+");
            if (bounds.length < 6 || steps.length < 3) {
                return defaults;
            }
            // abdul 27/07/2026 [import legacy maxima and the actual step file without cross-schema truncation]
            return new SuperPolyVarySettings(
                    Integer.parseInt(bounds[0]), Integer.parseInt(bounds[1]),
                    Integer.parseInt(bounds[2]), Integer.parseInt(bounds[3]),
                    Integer.parseInt(bounds[4]), Integer.parseInt(bounds[5]),
                    Integer.parseInt(steps[0]), Integer.parseInt(steps[1]),
                    Integer.parseInt(steps[2]), defaults.repetitions(),
                    defaults.colorCycle(), defaults.autoCover(),
                    defaults.autoSmallCover(), defaults.magnify(),
                    defaults.magnification());
        } catch (final IOException | RuntimeException exception) {
            return defaults;
        }
    }

    static void save(final Path file, final SuperPolyVarySettings settings) {
        final Properties properties = new Properties();
        properties.setProperty("version", VERSION);
        properties.setProperty("csMaximum",
                Integer.toString(settings.csMaximum()));
        properties.setProperty("osoMaximum",
                Integer.toString(settings.osoMaximum()));
        properties.setProperty("osnoMaximum",
                Integer.toString(settings.osnoMaximum()));
        properties.setProperty("csSideSumMaximum",
                Integer.toString(settings.csSideSumMaximum()));
        properties.setProperty("osoSideSumMaximum",
                Integer.toString(settings.osoSideSumMaximum()));
        properties.setProperty("osnoSideSumMaximum",
                Integer.toString(settings.osnoSideSumMaximum()));
        properties.setProperty("csStep", Integer.toString(settings.csStep()));
        properties.setProperty("osoStep", Integer.toString(settings.osoStep()));
        properties.setProperty("osnoStep",
                Integer.toString(settings.osnoStep()));
        properties.setProperty("repetitions",
                Integer.toString(settings.repetitions()));
        properties.setProperty("colorCycle",
                Boolean.toString(settings.colorCycle()));
        properties.setProperty("autoCover",
                Boolean.toString(settings.autoCover()));
        properties.setProperty("autoSmallCover",
                Boolean.toString(settings.autoSmallCover()));
        properties.setProperty("magnify",
                Boolean.toString(settings.magnify()));
        properties.setProperty("magnification",
                Double.toString(settings.magnification()));

        final Path parent = file.toAbsolutePath().getParent();
        try {
            Files.createDirectories(parent);
            final Path temporary = Files.createTempFile(
                    parent, file.getFileName().toString(), ".tmp");
            try {
                try (OutputStream output = Files.newOutputStream(temporary)) {
                    properties.store(output,
                            "BilliardsEverything SuperPolyVary settings");
                }
                // abdul 27/07/2026 [replace the complete settings generation atomically when supported]
                try {
                    Files.move(temporary, file,
                            StandardCopyOption.ATOMIC_MOVE,
                            StandardCopyOption.REPLACE_EXISTING);
                } catch (final AtomicMoveNotSupportedException exception) {
                    Files.move(temporary, file,
                            StandardCopyOption.REPLACE_EXISTING);
                }
            } finally {
                Files.deleteIfExists(temporary);
            }
        } catch (final IOException exception) {
            throw new RuntimeException(
                    "Unable to save SuperPolyVary settings.", exception);
        }
    }

    private static int integer(
            final Properties properties, final String name) {
        return Integer.parseInt(required(properties, name));
    }

    private static double decimal(
            final Properties properties, final String name) {
        return Double.parseDouble(required(properties, name));
    }

    private static boolean bool(
            final Properties properties, final String name) {
        final String value = required(properties, name);
        if (!value.equals("true") && !value.equals("false")) {
            throw new IllegalArgumentException(
                    "Invalid boolean property: " + name);
        }
        return Boolean.parseBoolean(value);
    }

    private static String required(
            final Properties properties, final String name) {
        final String value = properties.getProperty(name);
        if (value == null) {
            throw new IllegalArgumentException(
                    "Missing SuperPolyVary property: " + name);
        }
        return value;
    }
}
