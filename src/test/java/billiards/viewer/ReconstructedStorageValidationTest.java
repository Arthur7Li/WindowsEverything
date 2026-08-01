package billiards.viewer;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.List;

import billiards.geometry.Vector2;
import org.junit.jupiter.api.Test;

final class ReconstructedStorageValidationTest {
    private static final List<Vector2> SQUARE = List.of(
            Vector2.create(-0.1, -0.1),
            Vector2.create(0.1, -0.1),
            Vector2.create(0.1, 0.1),
            Vector2.create(-0.1, 0.1));

    @Test
    void provesPositiveConstantCosineOverWholeHull() {
        // abdul 27/07/2026 [exercise the interior Lipschitz proof, not only sampled vertices]
        assertEquals(
                ValidationStatus.VALID,
                Utils.validateReconstructedRegion(
                        "", "1 0 0", SQUARE));
    }

    @Test
    void provesAConcreteNegativeWitnessInvalid() {
        assertEquals(
                ValidationStatus.INVALID,
                Utils.validateReconstructedRegion(
                        "", "-1 0 0", SQUARE));
    }

    @Test
    void emptyMalformedAndBoundaryEvidenceRemainInconclusive() {
        // abdul 27/07/2026 [prevent empty or weak metadata from authorizing database deletion]
        assertEquals(
                ValidationStatus.INCONCLUSIVE,
                Utils.validateReconstructedRegion("", "", SQUARE));
        assertEquals(
                ValidationStatus.INCONCLUSIVE,
                Utils.validateReconstructedRegion(
                        "", "1 0", SQUARE));
        assertEquals(
                ValidationStatus.INCONCLUSIVE,
                Utils.validateReconstructedRegion(
                        "1 0 0", "", SQUARE));
    }
}
