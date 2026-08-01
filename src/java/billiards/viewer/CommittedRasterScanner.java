package billiards.viewer;

import billiards.geometry.ConvexPolygon;
import billiards.geometry.Location;
import billiards.geometry.Vector2;

import java.util.Collection;
import java.util.Optional;
import java.util.function.IntBinaryOperator;

/**
 * Camera-bound raster queries shared by hole-finding workflows.
 */
final class CommittedRasterScanner {
    private CommittedRasterScanner() {
    }

    static boolean isTransparentAtPoint(
            final int side,
            final double rx,
            final double ry,
            final PixelRadianMap camera,
            final IntBinaryOperator argb) {
        // abdul 27/07/2026 [reject coordinates outside the committed raster before any PixelReader access]
        final int pixelX = (int) Math.floor(camera.pixelX(rx));
        final int pixelY = (int) Math.floor(camera.pixelY(ry));
        return pixelX >= 0 && pixelX < side
                && pixelY >= 0 && pixelY < side
                && argb.applyAsInt(pixelX, pixelY) == 0;
    }

    static int countTransparent(
            final int side,
            final IntBinaryOperator argb) {
        int count = 0;
        for (int pixelX = 0; pixelX < side; pixelX++) {
            for (int pixelY = 0; pixelY < side; pixelY++) {
                if (argb.applyAsInt(pixelX, pixelY) == 0) {
                    count++;
                }
            }
        }
        return count;
    }

    static boolean canMutateCommittedRaster(
            final long committedGeneration,
            final long latestRequestedGeneration,
            final boolean ownsImage) {
        // abdul 27/07/2026 [prevent an incremental draw from being overwritten by an already-pending older snapshot]
        return ownsImage
                && committedGeneration == latestRequestedGeneration;
    }

    static Optional<Vector2> firstHole(
            final int side,
            final int xMin,
            final int xMax,
            final int yMin,
            final int yMax,
            final ConvexPolygon area,
            final Collection<Vector2> already,
            final PixelRadianMap camera,
            final IntBinaryOperator argb) {
        // abdul 27/07/2026 [interpret every sampled pixel through the camera committed with that raster]
        for (int pixelX = Math.max(0, xMin);
                pixelX < Math.min(side, xMax); pixelX++) {
            for (int pixelY = Math.max(0, yMin);
                    pixelY < Math.min(side, yMax); pixelY++) {
                if (argb.applyAsInt(pixelX, pixelY) != 0) {
                    continue;
                }
                final Vector2 point = Vector2.create(
                        camera.radianX(pixelX + 0.5),
                        camera.radianY(pixelY + 0.5));
                if (area.location(point.x, point.y) == Location.INSIDE
                        && !already.contains(point)) {
                    return Optional.of(point);
                }
            }
        }
        return Optional.empty();
    }
}
