package billiards.viewer;

import java.util.Arrays;

/**
 * Immutable plain-value snapshot of the Boyan search controls.
 */
// abdul 28/07/2026 [document the immutable request boundary that prevents worker reads of live JavaFX controls]
public record VarySearchRequest(
        int minimumMoves,
        int maximumMoves,
        int shots,
        int autoIterations,
        int autoStep,
        int maximumPrinting,
        boolean[] codeTypes) {
    public VarySearchRequest {
        if (minimumMoves < 0 || maximumMoves < minimumMoves) {
            throw new IllegalArgumentException("Invalid Vary move range.");
        }
        if (shots < 0 || autoIterations < 0 || autoStep < 0
                || maximumPrinting < 0) {
            throw new IllegalArgumentException(
                    "Vary counts must be nonnegative.");
        }
        if (codeTypes == null || codeTypes.length != 10) {
            throw new IllegalArgumentException(
                    "Exactly ten Vary code-type flags are required.");
        }
        codeTypes = codeTypes.clone();
    }

    @Override
    public boolean[] codeTypes() {
        return codeTypes.clone();
    }

    public boolean[] primaryTypes() {
        return Arrays.copyOf(codeTypes, 5);
    }

    public boolean[] allTypes() {
        return codeTypes();
    }
}
