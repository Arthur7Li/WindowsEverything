package billiards.wrapper;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

/**
 * Regression coverage for the Java/native Vary operation cancellation latch.
 */
public final class WrapperCancellationTest {

    /**
     * Verifies that cancellation survives repeated requests and is cleared
     * only when the owner explicitly begins a later native-Vary operation.
     */
    @Test
    public void cancellationRemainsLatchedUntilTheNextOperation() {
        // abdul 31/07/2026 [establish a clean operation boundary in both Java and native state]
        Wrapper.beginNativeVaryOperation();
        assertFalse(Wrapper.isVaryCancellationRequested());

        // abdul 31/07/2026 [exercise the same idempotent signal used by progress buttons and close controls]
        Wrapper.requestVaryCancellation();
        Wrapper.requestVaryCancellation();
        assertTrue(Wrapper.isVaryCancellationRequested());

        // abdul 31/07/2026 [prove only explicit admission of the next operation clears the Java latch]
        Wrapper.beginNativeVaryOperation();
        assertFalse(Wrapper.isVaryCancellationRequested());
    }
}
