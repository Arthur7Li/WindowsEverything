# Evidence-First Repairs and Optimization — 27 July 2026

## Purpose

This pass rechecked source-audit findings against the active Abdul Windows
fork, added regressions that distinguish the faulty and intended behavior, and
then repaired only bounded defects. It did not change database locations,
runtime memory defaults, dormant features, or the updater.

> Later status (28 July 2026): this file remains the exact record of the
> evidence-first cohort. The subsequent critical-repair campaign is documented
> in `CRITICAL-REPAIR-CHECKPOINT-2026-07-27.md`. That campaign removed the
> updater, added strict intersection certificates, and changed the normalized
> long-workload hash to `9f837d3bd0c612a7` solely through tighter certified
> root intervals. The `883e8b0c1317184e` hash and timings below are retained as
> historical pre-critical-repair evidence, not the current expected payload.

The pre-change baseline passed Java tests, native compilation, and 34 native
tests. That established that these findings were latent contract violations,
not existing test-suite failures.

## Repairs

### BUG-075: gradient JNA return ABI

Java declared `calculate_gradient` as returning `int`, while the C export
returned `float64_t`. Those types use different ABI return conventions on
supported platforms even though the function only returned status values
`1` and `-1`.

The C header and definition now return `int32_t`. Java's declaration and public
methods are unchanged. The Gradle Java test task now builds and loads the
backend from the same source tree, allowing a real JNA test to assert a
successful gradient report and a forced malformed-equation failure. A native
compile-time assertion independently locks the exported return type.

### BUG-048: Vary4 discarded vector differences

`Vector2D::sub` returns a new value. Four Vary4 sites called it and discarded
that value, so beam visibility and trail trimming used absolute endpoints
instead of point differences. The C++ code now assigns every subtraction
result, matching Java's value semantics.

The focused trajectory starts at angles `0.7, 0.8`, unfolds right and then
left, and requires the descendant beam width to be exactly `1.5` within
`1e-12`. The old code produced a different width and trail.

### BUG-046: Vary4 stale subtree beam

Native candidate admission tested the beam stored in the worker's fixed
subtree root. Descendants have different trails and bounds, so that root is
stale. Candidate admission now calls `frame.cbilliard.between`, matching the
current-node contract used by Java.

The translated two-step fixture demonstrates the exact distinction: the root
admits the descendant angle while the actual descendant rejects it. Complete
Java/native candidate parity remains part of the broader Vary4 audit because
separately tracked split-frontier and depth differences can also change whole
result sets.

### BUG-023: restricted `LeftRight` merge

The restricted unfolding overload merged task-local maps with `map::insert`.
When two worker chunks produced the same equation key, the later witness vector
was discarded. It also omitted the sorting used by the unrestricted overload.

Both overloads now use one append-per-key merge followed by deterministic
sorting. The regression duplicates a real `1 3 3` witness corpus, splits equal
keys across workers, and requires exact `CurvesLR` equality at one, two, and
four workers. The reported long-CS MRR test also passes at one and four workers.

### BUG-160 through BUG-163: Save V3 path and file transaction

The visible path was editable but ignored by Save Matching, Save Latest Vary,
and the destructive Clear command. Count parsing could escape the JavaFX
handler or accept nonpositive counts. Opening a writer made the new-file branch
unreachable, and manual close paths could leak the writer or leave a partial
append.

The dialog now:

- uses the visible typed or browsed path as the sole command input;
- uses a save dialog with a text-file filter;
- trims and validates the exact `1 <= count <= available` contract;
- snapshots the selected result list before file work;
- writes UTF-8 with one consistent prefix contract for new and existing files;
- stages output beside the destination and replaces it only after completion;
- clears only the currently visible destination through the same staged
  replacement boundary; and
- remembers path/count only after success.

Filesystem tests cover typed paths, missing and existing files, malformed and
out-of-range counts, unchanged data after invalid requests, and protection of
an earlier path during Clear. Disk work remains on JavaFX and is still tracked
separately as BUG-164.

## Optimization

### OPT-025: exact vector transform capacity

The vector `falgo::transform` produces exactly one output for every input but
previously grew its result repeatedly through `back_inserter`. It now reserves
`vec.size()` before transformation. It still does not resize, so empty inputs
remain valid and output elements do not need a default constructor.

The regression verifies size, order, empty input, and a result type whose
default constructor is deleted. The long-MRR benchmark is treated as a
correctness/performance smoke test rather than a claim that this small
allocation change dominates total MRR runtime.

## Validation

Completed during this pass:

- focused Java filesystem and JNA tests;
- 38 native tests, increased from the 34-test baseline;
- restricted curve parity at one, two, and four workers;
- exact translated Vary4 beam regression;
- exact reported long-CS slow MRR regression;
- Java compilation and rebuilt native shared/static libraries.

The five-sample baseline long-MRR matrix produced zero correctness failures and
one result hash, `883e8b0c1317184e`, across all worker counts:

| Workers | Valid samples | Median wall time | Range | Maximum working set |
| ---: | ---: | ---: | ---: | ---: |
| 1 | 5/5 | 9897.271 ms | 9860.666–9970.873 ms | 41,877,504 bytes |
| 2 | 5/5 | 6051.055 ms | 5858.683–6343.688 ms | 49,278,976 bytes |
| 4 | 5/5 | 4342.074 ms | 4324.645–4547.721 ms | 49,475,584 bytes |

Raw benchmark evidence is under the ignored
`build/benchmarks/20260727-173901-reported-long-cs-mrr-baseline/` directory.
These timings characterize the complete workload and do not isolate OPT-025.
The enforcing handbook audit passed at 4,081/4,081 production symbols with
zero explanatory gaps. Reader editions rebuilt successfully as a 2,321,363-byte
self-contained HTML file and a 10,865,685-byte PDF. The live bug register
parses as 321 unique IDs.

## Remaining Manual Checks

- Open Save V3, type a new `.txt` path without Browse, save two entries, and
  confirm only that typed file changes.
- Browse or type one file, replace the visible field with another, press Clear,
  and confirm the earlier file remains intact.
- Exercise representative Vary4 searches through the UI and compare normalized
  candidate sets while addressing the remaining Vary4 frontier/depth findings.
- Do not interpret this pass as resolving proof-topology findings such as
  BUG-034, BUG-038, or BUG-039.
