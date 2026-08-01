package billiards.viewer;

/**
 * Immutable publication and traversal options for one AutoPolyVary run.
 */
public record AutoPolyVaryOptions(
        boolean reverse,
        int printMode,
        int groupsToPrint,
        boolean addToAllPositive,
        boolean addToPlusMinus) {
    public AutoPolyVaryOptions {
        // abdul 27/07/2026 [validate all per-coordinate options before a worker generation starts]
        if (printMode < 0 || printMode > 2 || groupsToPrint < 0) {
            throw new IllegalArgumentException(
                    "Invalid AutoPolyVary publication options.");
        }
    }
}
