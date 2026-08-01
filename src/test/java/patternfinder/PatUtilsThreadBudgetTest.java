package patternfinder;

import billiards.viewer.Utils;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

// abdul 31/07/2026 [prevent PatternFinder from silently returning to an all-processor worker count]
final class PatUtilsThreadBudgetTest {
    @Test
    void usesViewerConfiguredWorkerBudget() {
        assertEquals(Utils.numThreads, PatUtils.numThreads);
    }
}
