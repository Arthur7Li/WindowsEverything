package billiards.viewer;

import org.junit.jupiter.api.Test;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

import static org.junit.jupiter.api.Assertions.assertTrue;

// arthur 06/08/2026 [keep the packaged launcher aligned with documented Gradle JVM memory defaults]
final class WindowsLauncherOptionsTest {
    @Test
    void packageScriptContainsRequiredMemoryOptions() throws IOException {
        final String script = Files.readString(Path.of("package-windows.bat"));

        assertTrue(script.contains("--java-options \"-Xms2g\""));
        assertTrue(script.contains("--java-options \"-Xmx6g\""));
        assertTrue(script.contains("--java-options \"-XX:MaxDirectMemorySize=2g\""));
        assertTrue(script.contains("--java-options \"-Xss2m\""));
    }
}
