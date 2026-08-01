package billiards.cover;

import static org.junit.jupiter.api.Assertions.assertTrue;

import billiards.codeseq.ClassifiedCodeSequence;

import org.eclipse.collections.impl.list.mutable.primitive.IntArrayList;
import org.junit.jupiter.api.Test;

final class CoverTripleTest {
    private static ClassifiedCodeSequence code(final int... values) {
        return ClassifiedCodeSequence.create(
                IntArrayList.newListWith(values)).get();
    }

    @Test
    void acceptsOnlyExactlyStableUnstableStable() {
        // abdul 27/07/2026 [cover every structural admission position before proof-artifact append]
        final ClassifiedCodeSequence stable = code(1, 1, 1);
        final ClassifiedCodeSequence unstable = code(2, 2);

        assertTrue(CoverTriple.fromCodes(new ClassifiedCodeSequence[] {
                stable, unstable, stable
        }).isPresent());
        assertTrue(CoverTriple.fromCodes(new ClassifiedCodeSequence[] {
                stable, stable, stable
        }).isEmpty());
        assertTrue(CoverTriple.fromCodes(new ClassifiedCodeSequence[] {
                unstable, unstable, stable
        }).isEmpty());
        assertTrue(CoverTriple.fromCodes(new ClassifiedCodeSequence[] {
                stable, unstable
        }).isEmpty());
        assertTrue(CoverTriple.fromCodes(new ClassifiedCodeSequence[] {
                stable, null, stable
        }).isEmpty());
    }
}
