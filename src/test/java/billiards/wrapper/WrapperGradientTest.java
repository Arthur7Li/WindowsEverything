package billiards.wrapper;

import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

public final class WrapperGradientTest {

    @Test
    public void gradientStatusCrossesJnaWithoutAbiCorruption() {
        // abdul 27/07/2026 [exercise the rebuilt native gradient status through the exact Java JNA declaration]
        final String details = Wrapper.calculateGradient("sin(x)", "5", "7", false);
        assertTrue(details.contains("min is"));

        assertThrows(
            RuntimeException.class,
            () -> Wrapper.calculateGradient("not-an-equation", "5", "7", false));
    }
}
