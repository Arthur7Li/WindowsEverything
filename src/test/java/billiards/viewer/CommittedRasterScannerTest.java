package billiards.viewer;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

import billiards.geometry.ConvexPolygon;
import billiards.geometry.Rectangle;
import org.junit.jupiter.api.Test;

final class CommittedRasterScannerTest {
    @Test
    void oldRasterCoordinatesCannotBeReinterpretedByNewCamera() {
        // abdul 27/07/2026 [reproduce the delayed-render old-image/new-map mismatch]
        final PixelRadianMap committedCamera =
                new PixelRadianMap(10);
        final PixelRadianMap laterCamera =
                new PixelRadianMap(committedCamera);
        laterCamera.translateXBy(1.0);
        laterCamera.translateYBy(1.0);
        final ConvexPolygon oldViewArea =
                Rectangle.create(-0.1, 0.1, -0.1, 0.1)
                        .toConvexPolygon();

        assertTrue(CommittedRasterScanner.firstHole(
                10, 0, 1, 0, 1, oldViewArea, List.of(),
                committedCamera, (x, y) -> 0).isPresent());
        assertTrue(CommittedRasterScanner.firstHole(
                10, 0, 1, 0, 1, oldViewArea, List.of(),
                laterCamera, (x, y) -> 0).isEmpty());
    }

    @Test
    void pointLookupRejectsBothRasterBoundariesBeforeReadingPixels() {
        final PixelRadianMap camera = new PixelRadianMap(10);
        final AtomicInteger reads = new AtomicInteger();

        // abdul 27/07/2026 [lock PolyVary sampling to valid committed pixel indices, including negative fractional coordinates]
        assertFalse(CommittedRasterScanner.isTransparentAtPoint(
                10, camera.radianX(-0.1), camera.radianY(5.0),
                camera, (x, y) -> {
                    reads.incrementAndGet();
                    return 0;
                }));
        assertFalse(CommittedRasterScanner.isTransparentAtPoint(
                10, camera.radianX(10.0), camera.radianY(5.0),
                camera, (x, y) -> {
                    reads.incrementAndGet();
                    return 0;
                }));
        assertEquals(0, reads.get());

        assertTrue(CommittedRasterScanner.isTransparentAtPoint(
                10, camera.radianX(2.5), camera.radianY(3.5),
                camera, (x, y) -> {
                    reads.incrementAndGet();
                    return 0;
                }));
        assertEquals(1, reads.get());
    }

    @Test
    void transparentCountUsesOnlyTheCommittedRasterExtent() {
        // abdul 27/07/2026 [give Fill Screen a deterministic committed-raster hole count]
        assertEquals(2, CommittedRasterScanner.countTransparent(
                2, (x, y) -> x == y ? 0 : 1));
    }

    @Test
    void pendingGenerationForcesAFullRenderInsteadOfIncrementalMutation() {
        // abdul 27/07/2026 [reproduce a late incremental result racing a pending full-render snapshot]
        assertTrue(CommittedRasterScanner.canMutateCommittedRaster(
                12, 12, true));
        assertFalse(CommittedRasterScanner.canMutateCommittedRaster(
                12, 13, true));
        assertFalse(CommittedRasterScanner.canMutateCommittedRaster(
                12, 12, false));
    }
}
