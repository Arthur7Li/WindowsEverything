package billiards.cover;

import billiards.codeseq.ClassifiedCodeSequence;
import billiards.codeseq.Storage;

import java.util.Optional;

/**
 * Structurally valid stable-unstable-stable cover triple.
 */
public record CoverTriple(
        ClassifiedCodeSequence stableNegative,
        ClassifiedCodeSequence unstable,
        ClassifiedCodeSequence stablePositive) {
    public CoverTriple {
        // abdul 27/07/2026 [make the proof-artifact triple lane structurally typed]
        if (stableNegative == null || unstable == null
                || stablePositive == null || !stableNegative.stable
                || unstable.stable || !stablePositive.stable) {
            throw new IllegalArgumentException(
                    "Cover triples require stable-unstable-stable components.");
        }
    }

    public static Optional<CoverTriple> fromCodes(
            final ClassifiedCodeSequence[] codes) {
        if (codes == null || codes.length != 3) {
            return Optional.empty();
        }
        try {
            return Optional.of(new CoverTriple(
                    codes[0], codes[1], codes[2]));
        } catch (final IllegalArgumentException exception) {
            return Optional.empty();
        }
    }

    public static Optional<CoverTriple> fromStorages(
            final Storage[] storages) {
        if (storages == null || storages.length != 3
                || storages[0] == null || storages[1] == null
                || storages[2] == null) {
            return Optional.empty();
        }
        return fromCodes(new ClassifiedCodeSequence[] {
                storages[0].classCodeSeq,
                storages[1].classCodeSeq,
                storages[2].classCodeSeq
        });
    }

    public ClassifiedCodeSequence[] codes() {
        return new ClassifiedCodeSequence[] {
                stableNegative, unstable, stablePositive
        };
    }

    public String artifactText() {
        return stableNegative + ", " + unstable + ", " + stablePositive;
    }
}
