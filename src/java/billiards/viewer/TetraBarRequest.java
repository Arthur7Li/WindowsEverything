package billiards.viewer;

import javaslang.Tuple2;

import java.util.List;
import java.util.Objects;

/**
 * Immutable input and publication policy for one Bar/Tetrahedron generation.
 */
public record TetraBarRequest(
        List<Tuple2<Double, Double>> originalPoints,
        List<Tuple2<Double, Double>> samplePoints,
        int groupSize,
        int maximumPrinting,
        boolean draw,
        boolean addToCover,
        double x2,
        double y2,
        double x3,
        double y3,
        double line1Cut,
        double line2Cut,
        VarySearchRequest searchRequest) {
    public TetraBarRequest {
        // abdul 27/07/2026 [make TetraBar samples and Vary controls one immutable generation]
        originalPoints = List.copyOf(Objects.requireNonNull(originalPoints));
        samplePoints = List.copyOf(Objects.requireNonNull(samplePoints));
        searchRequest = Objects.requireNonNull(searchRequest);
        if (groupSize != 2 && groupSize != 3) {
            throw new IllegalArgumentException(
                    "TetraBar group size must be two or three.");
        }
        if (samplePoints.size() % groupSize != 0
                || originalPoints.size() * groupSize != samplePoints.size()) {
            throw new IllegalArgumentException(
                    "TetraBar point collections do not form complete groups.");
        }
        if (maximumPrinting < 0) {
            throw new IllegalArgumentException(
                    "TetraBar print count must be nonnegative.");
        }
        if (!Double.isFinite(x2) || !Double.isFinite(y2)
                || !Double.isFinite(x3) || !Double.isFinite(y3)
                || !Double.isFinite(line1Cut) || !Double.isFinite(line2Cut)) {
            throw new IllegalArgumentException(
                    "TetraBar Vary geometry must be finite.");
        }
    }
}
