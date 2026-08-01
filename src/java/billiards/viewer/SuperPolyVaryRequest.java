package billiards.viewer;

import billiards.geometry.ConvexPolygon;

import java.util.Objects;

/**
 * Complete immutable generation request consumed by SuperPolyVary callbacks.
 */
public record SuperPolyVaryRequest(
        ConvexPolygon polygon,
        SuperPolyVarySettings settings,
        boolean automatic,
        int subdivisions,
        int subdivisionStep,
        int startIndex,
        int lineStep,
        int endIndex,
        AutoPolyVaryOptions automaticOptions,
        VarySearchRequest searchRequest) {
    public SuperPolyVaryRequest {
        // abdul 27/07/2026 [bind schedule, mode, line range, and search values to one generation]
        polygon = Objects.requireNonNull(polygon);
        settings = Objects.requireNonNull(settings);
        automaticOptions = Objects.requireNonNull(automaticOptions);
        searchRequest = Objects.requireNonNull(searchRequest);
        if (subdivisions < 0 || lineStep == 0) {
            throw new IllegalArgumentException(
                    "Invalid SuperPolyVary subdivision or line step.");
        }
    }
}
