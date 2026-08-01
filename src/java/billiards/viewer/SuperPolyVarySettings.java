package billiards.viewer;

/**
 * Persisted dialog values for SuperPolyVary.
 */
public record SuperPolyVarySettings(
        int csMaximum,
        int osoMaximum,
        int osnoMaximum,
        int csSideSumMaximum,
        int osoSideSumMaximum,
        int osnoSideSumMaximum,
        int csStep,
        int osoStep,
        int osnoStep,
        int repetitions,
        boolean colorCycle,
        boolean autoCover,
        boolean autoSmallCover,
        boolean magnify,
        double magnification) {
    public SuperPolyVarySettings {
        // abdul 27/07/2026 [validate the complete persisted Super schedule as one value]
        if (csMaximum < 0 || osoMaximum < 0 || osnoMaximum < 0
                || csSideSumMaximum < 0 || osoSideSumMaximum < 0
                || osnoSideSumMaximum < 0 || repetitions < 0) {
            throw new IllegalArgumentException(
                    "SuperPolyVary bounds and repetitions must be nonnegative.");
        }
        if (!Double.isFinite(magnification)
                || (magnify && magnification <= 0.0)) {
            throw new IllegalArgumentException(
                    "SuperPolyVary magnification must be finite and positive when enabled.");
        }
    }

    public static SuperPolyVarySettings defaults() {
        return new SuperPolyVarySettings(
                300, 50, 36, 800, 300, 150,
                0, 0, 0, 0, true, true, false, false, 2.0);
    }
}
