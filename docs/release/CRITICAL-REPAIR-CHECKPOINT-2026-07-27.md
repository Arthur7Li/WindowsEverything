# Critical Repair Campaign Checkpoint — 2026-07-27

> **State:** completed at checkpoint 11 on 2026-07-28. All 23 active,
> in-scope critical rows are fixed and validated. `BUG-166` and `BUG-168`
> remain open because the user explicitly classified Small Cover as dormant.
> This file is the authoritative continuation record; no commit or push was
> made.

## Repository identity and authority

- Repository:
  `C:\Users\Owner\Documents\Programming Projects\research\Abdul-Windows-fork-BilliardsEverything`
- Branch: `main`
- Baseline/working HEAD:
  `0d2be2093bed5b4673b855123f1ff8ce16d2fa20`
- No commit, push, release publication, installer execution, database
  relocation, or runtime-memory-default change has been made.
- The working tree contains the earlier evidence-first repair pass and this
  critical campaign together. Preserve both.
- Every hand-written production/test/build/script edit must retain a nearby
  exact dated comment of the form:
  `abdul 27/07/2026 [what changed and why]` or
  `abdul 28/07/2026 [what changed and why]`.

## User decisions that define this campaign

- Repair every active critical finding unless it is dormant.
- `BUG-166` and `BUG-168` are Small Cover findings and are explicitly dormant
  for this campaign. Do not implement them unless the user changes scope.
- Disable automatic updating. Do not synthesize keys, signatures, or a new
  installer trust system.
- Preserve Abdul-specific behavior and the repository's established runtime
  memory defaults.
- Leave the completed checkpoint and exact continuation information durable.

The active campaign set is:

`BUG-034`, `BUG-037`, `BUG-038`, `BUG-039`, `BUG-177`, `BUG-178`,
`BUG-181`, `BUG-185`, `BUG-186`, `BUG-187`, `BUG-188`, `BUG-192`,
`BUG-202`, `BUG-203`, `BUG-205`, `BUG-212`, `BUG-216`, `BUG-217`,
`BUG-219`, `BUG-220`, `BUG-229`, `BUG-231`, and `BUG-243`.

## Pre-existing dirty state that must be preserved

These paths were already dirty before the critical campaign and are user or
runtime state, not disposable repair artifacts:

- `.gradle/8.0/executionHistory/executionHistory.lock`
- `cover/info.txt`
- `garbage.txt`
- `tmp/cover_magnifications.txt`
- `tmp/cover_stables.txt`

In particular, `cover/info.txt` currently appears as a very large deletion in
`git diff --stat`; do not reconstruct or revert it without explicit user
direction.

The forced test run at this checkpoint also dirtied other tracked `.gradle`
cache binaries and locks. Those generated changes are not product changes.
When cleaning them later, preserve the pre-existing
`executionHistory.lock` state and do not use a broad reset/checkout that could
erase user changes.

## Earlier evidence-first repairs still present

The uncommitted tree already contained a completed earlier cohort. Preserve
these repairs and their regressions while continuing:

- `BUG-075`: native gradient ABI status now matches Java JNA.
- `BUG-048`: Vary4 retains all translated vector differences.
- `BUG-023`: curve merging preserves duplicate keys and worker parity.
- `OPT-025`: vector transforms reserve exact output capacity.
- `BUG-046`: Vary4 uses the current descendant beam.
- `BUG-160` through `BUG-163`: validated/staged SaveV3 replacement through
  `SaveV3FileService`.

That cohort previously passed the full fast gate, the exact slow reported
long-CS MRR case, worker-parity samples, and handbook enforcement. Its release
record is `EVIDENCE-FIRST-REPAIRS-2026-07-27.md`.

## Critical work implemented in the current tree

The following is implemented but not yet entitled to a final campaign-wide
“fixed” declaration because the full gates and the outstanding items in the
next section remain.

### Certified native geometry: BUG-034, BUG-037, BUG-038, BUG-039

- `gradient.cpp` dispatches `(0, pi/2)` and `(pi/2, 0)` derivative recovery to
  their matching exact evaluators, checks normalization divisions, derives a
  safe multiplicity bound from stored frequencies, and rejects the ambiguous
  `(0, 0)` case.
- `intersection.hpp` uses interval/Krawczyk common-root certification before
  accepting a scalar Newton candidate. It retains the widest successfully
  certified box instead of assuming a fixed `1e-25` error box.
- Nonlinear boundary candidates must belong to the connected regular
  implicit-curve component/active exact cosine factor, rather than merely
  lying in the infinite slab between chord endpoints.
- `refine.cpp` adaptively certifies nonzero sign over linear or continued
  nonlinear boundary pieces before equal-sign endpoints are accepted.
  Unresolved topology fails closed before publication.
- Failed Newton attempts no longer end retry-basin exploration prematurely.
- New native fixtures cover both special corners, a certified line
  intersection, disjoint equation-zero ranges, a wrong nonlinear branch, and
  concealed double crossings.
- The native suite passed 44 tests immediately after this cohort.

Primary files:

- `src/backend/cpp/gradient.cpp`
- `src/backend/cpp/refine.cpp`
- `src/backend/headers/evalf.hpp`
- `src/backend/headers/intersection.hpp`
- `src/test/headers/gradient_test.hpp`
- `src/test/headers/intersection_test.hpp`

### Application-owned operation lifetime: BUG-216, BUG-217, BUG-243

- `OperationRegistry` provides generation IDs, exclusive operation keys,
  Future/executor ownership, exactly-once terminal transition, and
  asynchronous cancel/shutdown/await behavior.
- A terminal handle remains registered until its owned executors have
  actually terminated; this prevents a nominal completion from allowing an
  overlapping generation while child pools are still alive.
- `Main` and `Viewer` stop admission, cancel, and await registered operations
  before the native pool is destroyed/application exit completes.
- VaryL and Middle VaryL use exclusive `"vary-l"` generations, own their
  storage/shot pools and submitted tasks, and terminate on success,
  cancellation, failure, rejected execution, and late progress cancellation.
- Direct PolyVary, AutoPolyVary, and SuperPolyVary use exclusive
  `"poly-vary"` generations and operation-owned pools/tasks. Failure and
  cancellation now route through one terminal owner.
- Tetra/Bar uses an exclusive `"tetra-bar"` generation with owned outer and
  shot pools.
- Cover-load and cover-artifact paths have operation owners rather than
  untracked per-action executor lifetime.

Primary new file and test:

- `src/java/billiards/viewer/OperationRegistry.java`
- `src/test/java/billiards/viewer/OperationRegistryTest.java`

### Immutable worker requests: BUG-177, BUG-178, BUG-181, BUG-185,
BUG-202, BUG-203, BUG-205

- `VarySearchRequest` snapshots all Boyan search controls on JavaFX and is
  passed through Vary, VaryL, PolyVary, AutoPolyVary, and Tetra/Bar worker
  calls.
- One-point VaryL builds a separate non-persistent request, does not overwrite
  the saved multi-point file/list, clears stale result state per invocation,
  and rejects a nested visible-dialog launch.
- `TetraBarRequest` captures all bounds/search values and uses a private
  per-generation accumulator. Workers do not read live JavaFX fields.
- SuperPolyVary now has a complete immutable `SuperPolyVaryRequest` including
  settings, auto/manual choice, subdivision schedule, line range, Boyan
  request, publication options, and automatic recursion options.
- Super settings have a dedicated versioned
  `SuperPolyVaryRequest.v1.properties` schema, atomic complete writes, correct
  legacy bounds/step import, and no truncation collision with ordinary
  PolyVary.
- `AutoPolyVaryOptions` freezes reverse/mode/group/iteration-publication
  choices so recursion does not reread a reopened dialog.

Primary new files:

- `src/java/billiards/viewer/VarySearchRequest.java`
- `src/java/billiards/viewer/TetraBarRequest.java`
- `src/java/billiards/viewer/SuperPolyVarySettings.java`
- `src/java/billiards/viewer/SuperPolyVarySettingsStore.java`
- `src/java/billiards/viewer/SuperPolyVaryRequest.java`
- `src/java/billiards/viewer/AutoPolyVaryOptions.java`
- `src/test/java/billiards/viewer/SuperPolyVarySettingsStoreTest.java`

### Typed reconstructed-storage validation: BUG-192

- `ValidationStatus` distinguishes `VALID`, `INVALID`, and `INCONCLUSIVE`.
- `Utils.verifyInfo` evaluates native-populated `sinEquations` and
  `cosEquations`, not the intentionally empty legacy fields.
- Parsing requires complete finite coefficient triples.
- A negative point sample is typed `INVALID`.
- `VALID` requires a conservative whole-hull Lipschitz disk lower bound above
  a scaled tolerance. Empty, malformed, boundary, or otherwise insufficient
  evidence is `INCONCLUSIVE`.
- `DrawPictureTaskUseLRTest` deletes a database row only for typed `INVALID`;
  `INCONCLUSIVE` falls back to the slow path without destructive deletion.

Primary files:

- `src/java/billiards/viewer/ValidationStatus.java`
- `src/java/billiards/viewer/Utils.java`
- `src/java/billiards/viewer/DrawPictureTaskUseLRTest.java`
- `src/test/java/billiards/viewer/ReconstructedStorageValidationTest.java`

### Cover triple and artifact publication: BUG-212, BUG-219, BUG-220

- `CoverTriple` admits exactly three complete
  stable–unstable–stable components and provides the proof-artifact text.
- The Viewer validates input triples with their source index before work is
  scheduled; computed incomplete/wrong-type triples are discarded and only a
  typed `CoverTriple` is appended.
- Singles and triples use private inner pools under one aggregate registered
  load operation, removing their race to shut down one shared pool.
- `CoverArtifactService` merges into a unique sibling staging directory,
  validates the seven required regular files, records sizes/SHA-256 in
  `manifest.sha256`, atomically switches directory generations when supported,
  restores the previous generation on failure, keeps one rollback generation,
  and serializes publication with a shared lock.
- The Viewer merge action is background/registered and does not pre-delete the
  live cover. `CoverWindow` uses the same artifact lock for its info write.

Primary files:

- `src/java/billiards/cover/CoverTriple.java`
- `src/java/billiards/viewer/CoverArtifactService.java`
- `src/java/billiards/viewer/DrawPictureTaskTriples.java`
- `src/java/billiards/viewer/CoverWindow.java`
- `src/java/billiards/viewer/Viewer.java`
- `src/test/java/billiards/cover/CoverTripleTest.java`
- `src/test/java/billiards/viewer/CoverArtifactServiceTest.java`

### Render snapshots and camera/raster pairing: BUG-229, BUG-231

- Full Viewer rendering captures a `RenderSnapshot` on JavaFX containing a
  copied camera, ordered regions/colors, render toggles, parsed offset, fills,
  cover rectangles/colors, every bound polygon/color group, and OBO state.
- Background full-render helpers consume the snapshot/camera/offset rather
  than rereading controls or mutable Viewer collections.
- Every render request receives a generation, including the synchronous path.
- Commit stores a `CommittedRaster` consisting of the image, copied camera,
  and generation.
- `findHole`/`findHoles`, direct PolyVary sampling, and AutoPolyVary recursive
  sampling use a matching committed image/camera pair and fail closed when no
  committed pair exists.
- `CommittedRasterScannerTest` demonstrates that an old raster is interpreted
  by its old camera rather than a newly mutated camera.

Primary files:

- `src/java/billiards/viewer/Viewer.java`
- `src/java/billiards/viewer/CommittedRasterScanner.java`
- `src/test/java/billiards/viewer/CommittedRasterScannerTest.java`

### Automatic updater disabled: BUG-186, BUG-187, BUG-188

- The update button is disabled and labeled `Updates disabled`.
- Its tooltip directs users to a manual trusted installation path.
- Viewer no longer imports or invokes `Updater`.
- The unauthenticated/destructive updater implementation and production
  scripts are deleted:
  - `src/java/billiards/viewer/Updater.java`
  - `updater.bat`
  - `updater.sh`
  - `app/updater.bat`
  - `app/updater.sh`

## Validation completed at this checkpoint

The following focused command was forced to execute, not merely accepted from
Gradle's up-to-date cache:

```powershell
.\gradlew.bat --no-daemon test --rerun-tasks `
  --tests billiards.viewer.OperationRegistryTest `
  --tests billiards.viewer.SuperPolyVarySettingsStoreTest `
  --tests billiards.viewer.ReconstructedStorageValidationTest `
  --tests billiards.cover.CoverTripleTest `
  --tests billiards.viewer.CoverArtifactServiceTest `
  --tests billiards.viewer.CommittedRasterScannerTest
```

Result:

- `BUILD SUCCESSFUL in 1m 43s`
- all six Gradle actions executed;
- fresh `compileJava`, native compile/link, and `compileTestJava` completed;
- 11 focused tests ran, with 0 skipped, 0 failures, and 0 errors:
  - `CoverTripleTest`: 1
  - `CommittedRasterScannerTest`: 1
  - `CoverArtifactServiceTest`: 2
  - `OperationRegistryTest`: 2
  - `ReconstructedStorageValidationTest`: 3
  - `SuperPolyVarySettingsStoreTest`: 2
- Compilation emitted nine existing-style warnings (`this-escape`,
  deprecated `TableView.CONSTRAINED_RESIZE_POLICY`, and a dangling Viewer
  doc comment) but no errors.

A preceding identical command without `--rerun-tasks` also returned
`BUILD SUCCESSFUL`, but all tasks were up-to-date; use the forced run above as
the checkpoint evidence.

## Mandatory continuation issues

### 1. BUG-217 BoyanMenu gap completed; repository-wide audit remains

The previously identified BoyanMenu lifecycle gap is now implemented:

- Viewer constructs BoyanMenu only after its `OperationRegistry` is assigned;
- ordinary Vary, Middle Vary, and Vary3B share the exclusive `boyan-vary`
  operation key;
- each asynchronous generation owns its outer executor, inner executor,
  JavaFX Task, and submitted Future;
- success, cancellation, failure, rejected submission, overlap, and
  application shutdown route through the registered terminal owner;
- inner worker interruption/failure is propagated rather than printed and
  converted into partial success;
- synchronous Vary4 reuses one caller-owned helper pool and releases it from a
  `finally` block.

The forced focused command
`.\gradlew.bat --no-daemon test --rerun-tasks --tests billiards.viewer.OperationRegistryTest`
passed after fresh Java/native compilation in 1m18s. Three tests ran with zero
skips/failures/errors, including a regression proving an exclusive key remains
reserved until an owned executor actually terminates.

Before marking `BUG-217` fixed, still perform the planned repository-wide
audit of active-workflow Threads, executors, and JavaFX Tasks and run the full
application/native gates.

### 2. BUG-229/BUG-231 incremental-render edges completed

- committed render context now includes the region/bounds/OBO images plus the
  exact copied camera, render options, offset, and generation;
- direct region and bounds mutation uses that committed context rather than
  the live camera/control state;
- if a newer full render is pending, an incremental result requests a new
  complete snapshot instead of mutating an image that the pending snapshot
  could overwrite;
- Fill Screen counts holes in one committed raster, persists that raster's
  exact view rectangle, and requests a complete generation render rather than
  recoloring the current image directly;
- direct and automatic PolyVary sampling rejects every coordinate outside the
  committed raster before invoking `PixelReader`.

The forced focused command
`.\gradlew.bat --no-daemon test --rerun-tasks --tests billiards.viewer.CommittedRasterScannerTest`
passed after fresh Java/native compilation in 1m18s. Four tests ran with zero
skips/failures/errors, covering old-image/new-camera interpretation, both
raster boundaries, deterministic transparent counting, and the
pending-generation incremental-mutation gate.

The repository-wide audit and full application/native gates remain before
`BUG-229`/`BUG-231` receive final tracker status.

### 3. CoverTriple source/test are now visible

The broad `.gitignore` rule was anchored from `cover/` to `/cover/`, preserving
the ignored root runtime artifact while allowing Java package paths to be
tracked. These two essential files now appear normally in `git status`:

- `src/java/billiards/cover/CoverTriple.java`
- `src/test/java/billiards/cover/CoverTripleTest.java`

This visibility repair is complete; retain the anchored rule.

### 4. Full validation and documentation remain

The campaign has **not** yet run the final full fast suite, the slow native
gate, worker-parity checks, handbook audit, or a clean-output build after all
critical edits.

The critical rows listed above still say `open` in
`docs/codex-project-study/bug-register.csv`. This is intentional at the
checkpoint. Existing edits in the handbook, coverage CSV, release README, and
significant-changes file belong chiefly to the preceding repair cohort; merge
the critical evidence carefully rather than overwriting them.

## Ordered next actions

1. Read repository `AGENTS.md`, this file, and
   `C:\tmp\abdul-repair-workstate-2026-07-27.md`.
2. Run `git status --short` and verify branch/HEAD still match this checkpoint.
3. Fix `.gitignore` visibility for the two `CoverTriple` files.
4. Finish the BoyanMenu/application-operation migration described above.
5. Close the remaining incremental-render/camera-pair paths and add focused
   regressions.
6. Audit every `new Thread`, `Executors.new*`, `PriorityExecutor`, and
   background JavaFX `Task` in active critical workflows. Synchronous helpers
   may use local pools only when shutdown is guaranteed before return.
7. Re-run focused lifecycle, request, validation, artifact, triple, and raster
   tests with `--rerun-tasks`.
8. Run the complete fast gate:

   ```powershell
   .\gradlew.bat --no-daemon compileJava backendSharedLibrary test testBackend
   ```

9. Run the native slow gate and the repository's documented worker-parity
   benchmark, preserving runtime-memory defaults.
10. Run handbook coverage/enforcement using the repository-provided tooling.
    Add coverage rows for all new production symbols and remove/retire current
    `Updater` symbols rather than leaving stale live-source claims.
11. Only after evidence passes, update each active critical tracker row with
    the exact fix, regression, and command evidence. Keep `BUG-166` and
    `BUG-168` open/deferred-dormant.
12. Complete the release/significant-change/handbook narrative, including the
    five updater deletions and the staged artifact trust boundary.
13. Clean only generated files created by this pass. Preserve the five
    pre-existing dirty paths and all unrelated user changes.
14. Do not commit or push without explicit user authorization.

## Worktree notes at pause

- All campaign changes are uncommitted.
- Normal `git status --short` currently reports production, test,
  documentation, updater-deletion, and Gradle-cache changes; use it as the
  source of truth rather than this prose for the final file list.
- The ignored `CoverTriple` source/test require the special check above.
- No further production change was started after the forced focused test.
- The separate compaction-safe checkpoint is:
  `C:\tmp\abdul-repair-workstate-2026-07-27.md`.

## Active-workflow audit checkpoint 2 - 2026-07-27

This section supersedes the earlier "Mandatory continuation issues" and
"Ordered next actions" where they describe Boyan, Cover, Cycle, raster-edge,
or `CoverTriple` work that is now complete. It is the current resumption point.

### Exact repository state

- branch: `main`
- HEAD: `0d2be2093bed5b4673b855123f1ff8ce16d2fa20`
- all campaign edits remain uncommitted;
- the user/pre-existing dirty files remain present and must be preserved:
  `.gradle/8.0/executionHistory/executionHistory.lock`, `cover/info.txt`,
  `garbage.txt`, `tmp/cover_magnifications.txt`, and
  `tmp/cover_stables.txt`;
- Gradle also dirtied cache binaries/locks during the forced builds. Do not
  use a broad restore/reset to remove them.

### Additional lifecycle repairs completed

- `CoverWindow` now admits MRR and All calculations under the exclusive
  `cover-artifact` operation key, tracks each JavaFX Task and submitted Future,
  and owns its outer executor.
- Closing Cover cancels its active generation and requests native backend
  cancellation.
- MRR and All native artifact writes use the same
  `CoverArtifactService` lock as merge publication.
- Cover stable cleaning no longer hides a `ForkJoinPool` inside the registered
  wrapper Task. Task construction now occurs after operation admission and
  that nested fan-out pool is owned by the same operation. Interrupted or
  failed fan-out stops queued work and preserves interrupt/root-cause
  evidence.
- `CycleVaryWindow` now receives the application `OperationRegistry`, admits
  one exclusive `cycle-vary` generation, and owns its draw executor plus every
  per-repetition storage/shot executor. The JavaFX Task and submitted Future
  are tracked; success, cancel, failure, rejection, and window close all reach
  an operation terminal state.
- Cycle Vary screen movement uses a nonpersistent caller executor so the
  render commits before raster sampling. It reads the committed view rectangle
  and checked committed-raster transparency rather than combining the live
  map with an older image.
- The iteration-pattern lookup no longer creates an unregistered one-off SQL
  executor. It now uses the Viewer registry, an exclusive
  `iterate-pattern-lookup` generation, a tracked Future, an owned executor,
  guarded JavaFX publication, try-with-resources for `ResultSet`, and
  cancellation on child-window close. All `IterateToLimitWindow` construction
  paths pass the registry, including Cycle/Auto/Poly result-sink paths.

### Audit classification so far

- `Main`'s fixed executor is application-owned and joined before native pool
  destruction.
- Viewer render jobs use that shared executor and its render Future is
  cancelled/coalesced before the shared executor is joined.
- Boyan Vary/Vary3B, Cover, Cycle Vary, TetraBar, VaryL/MiddleVaryL,
  direct/Auto/Super PolyVary, mixed code loading, Cover merge, and Tetra
  workflows now have explicit operation owners.
- Vary4 and the Viewer iteration helpers that call `Utils.runAndWait` are
  synchronous at their UI boundary; Vary4 has guaranteed local-pool cleanup.
- `DrawPictureTaskShowLR`, `DontDrawPictureTask`,
  `DrawPictureTaskUseLR`, and `DrawPictureTaskUseLRTest` own their internal
  pools and release them from `finally`.
- `Utils.shutdownExecutorAsync` uses only a daemon join helper. Remaining call
  sites must still be classified during the final audit, but the helper itself
  cannot retain the process.

### Fresh evidence at this checkpoint

The following compile passed after the Cover/lookup lifecycle additions:

```powershell
.\gradlew.bat --no-daemon compileJava
```

Result: `BUILD SUCCESSFUL in 6s`; only existing deprecation,
constructor-escape, and dangling-doc warnings were reported.

The forced combined regression command was:

```powershell
.\gradlew.bat --no-daemon test --rerun-tasks --tests billiards.viewer.OperationRegistryTest --tests billiards.viewer.CommittedRasterScannerTest --tests billiards.viewer.CoverArtifactServiceTest
```

Result: `BUILD SUCCESSFUL in 1m 14s`, with fresh Java/native/test compilation.
JUnit XML records 10 tests, zero skipped, zero failures, and zero errors:

- `OperationRegistryTest`: 3;
- `CommittedRasterScannerTest`: 4;
- `CoverArtifactServiceTest`: 3.

### Exact continuation order

1. Re-read `AGENTS.md`, this section, and
   `C:\tmp\abdul-repair-workstate-2026-07-27.md`.
2. Continue the repository-wide active-work audit. First inspect
   `IterateToLimitWindow.run()` local-executor cleanup and every remaining
   `Utils.shutdownExecutorAsync` call for guaranteed terminal ownership.
3. Inspect Cycle/Cover/Viewer terminal handlers for exactly-once completion,
   cancellation, late-publication guards, and child-pool termination.
4. Audit every changed handwritten code/test/build/config hunk for a nearby
   exact `abdul 27/07/2026 [reason]` comment.
5. Re-run all focused critical suites (not only the three in this checkpoint).
6. Run the full clean Java/native/fast gate, `testBackendSlow`, and documented
   1/2/4-worker parity benchmark while preserving the required memory defaults.
7. Run handbook coverage enforcement and update coverage for every new symbol
   and the removed updater.
8. Only after all gates pass, mark the active critical bug-register rows
   fixed and write final handbook/release/significant-change evidence.
   `BUG-166` and `BUG-168` remain open and explicitly dormant.
9. Clean only generated files created by this campaign. Preserve the five
   pre-existing dirty paths and unrelated user work.
10. Do not commit or push without explicit user authorization.

No production edit was started after the successful forced combined cohort.

## Terminal-publication audit checkpoint 3 - 2026-07-28

This is the current compaction/resumption point and supersedes checkpoint 2's
next-action list where the items below are now complete.

### Additional defects closed

- `IterateToLimitWindow.run()` now releases its synchronous fixed pool from a
  `finally` path. Interrupted waits restore the interrupt, cancel sibling
  Futures, suppress result publication, and force-stop queued work; unchecked
  exits can no longer leave the pool alive.
- `OperationHandle.permitsPublication()` centralizes the rule that JavaFX
  callbacks may publish only while both the operation and application
  admission lifetime remain active.
- PolyVary, VaryL, and CycleVary Tasks now check cancellation both before
  queueing a JavaFX partial and again inside the queued callback.
- Boyan Vary/Vary3B, Cover, CycleVary, VaryL, direct/Auto/Super PolyVary,
  TetraBar, mixed singles/triples loading, and Cover merge suppress success,
  partial, failure-dialog, render, recursion, and schedule publication after
  registry/window/application cancellation.
- User-requested graceful cancellation still preserves completed partials;
  registry-initiated hard cancellation does not publish them.
- Invalid CycleVary print configuration is now terminal rather than returning
  with its draw/storage/shot pools registered and alive.
- CycleVary's stable-cleaning `ForkJoinPool` is now owned by the same
  operation, matching the ordinary Cover calculation path.
- CycleVary schedule inputs that can fail parsing are validated before
  operation admission, and later repetition-listener failures cancel the
  generation.
- Invalid AutoPolyVary ranges now cancel the admitted generation rather than
  retaining an idle exclusive key.

### Active-workflow executor classification complete

The final source scan classified every `new Thread`, `Executors.new*`,
`PriorityExecutor`, `ForkJoinPool`, JavaFX Task, and
`shutdownExecutorAsync` site in the active Viewer workflows:

- asynchronous Boyan, Cover, Cycle, lookup, TetraBar, VaryL, PolyVary,
  Auto/Super, code-load, and merge resources are operation-owned;
- Main's shared render/task executor is joined before native-pool
  destruction;
- Vary4 and legacy Viewer iteration helpers are synchronous at their caller
  boundary and guarantee local-pool shutdown;
- task-private drawing pools release from `finally`;
- the remaining shutdown helper starts only a daemon join thread and cannot
  retain the process.

PatternFinder is a separate program selection and is not part of the
Viewer-specific `BUG-217` repair scope.

### Fresh forced evidence

`.\gradlew.bat --no-daemon compileJava` passed in 8s after the terminal guard
changes.

The complete focused critical/evidence cohort then ran with:

```powershell
.\gradlew.bat --no-daemon test --rerun-tasks --tests billiards.cover.CoverTripleTest --tests billiards.viewer.CommittedRasterScannerTest --tests billiards.viewer.CoverArtifactServiceTest --tests billiards.viewer.IterateToLimitWindowTest --tests billiards.viewer.OperationRegistryTest --tests billiards.viewer.ReconstructedStorageValidationTest --tests billiards.viewer.SaveV3FileServiceTest --tests billiards.viewer.SuperPolyVarySettingsStoreTest --tests billiards.viewer.ViewerAutoPolyVaryTest --tests billiards.wrapper.WrapperGradientTest
```

Result: `BUILD SUCCESSFUL in 1m 40s`; all six Gradle actions executed with
fresh Java/native/test compilation. JUnit XML records 27 tests, zero skipped,
zero failures, and zero errors across the 10 named suites.

### Exact continuation

1. Finish the hunk-level Abdul annotation/diff hygiene audit.
2. Run the clean full Java/native/fast gate.
3. Run `testBackendSlow`.
4. Run the documented 1/2/4-worker benchmark and compare deterministic
   digests.
5. Run handbook coverage enforcement/build; repair coverage/docs as needed.
6. Update all active critical tracker rows and final release/handbook
   narrative from exact evidence. Keep `BUG-166`/`BUG-168` open/dormant.
7. Remove only campaign-generated cache changes while preserving the five
   pre-existing dirty files; do not commit or push.

## Clean full-gate checkpoint 4 - 2026-07-28

The clean Java/native/fast gate completed successfully:

```powershell
.\gradlew.bat --no-daemon clean compileJava backendSharedLibrary test testBackend
```

Exact result:

- `BUILD SUCCESSFUL in 2m 51s`;
- all 12 Gradle actions executed after `clean`;
- JUnit XML: 28 tests, 0 skipped, 0 failures, 0 errors;
- Boost native suite: 44 test cases, `*** No errors detected`;
- Java compilation emitted the same nine known warnings recorded above;
- the intentional malformed-gradient regression printed its expected
  `unable to parse equation: not-an-equation` diagnostic and passed.

No repair failure is outstanding from the clean fast gate. Next run
`testBackendSlow`, then the documented worker-parity benchmark and handbook
gates.

## Slow-native repair checkpoint 5 - 2026-07-28

The first post-repair slow gate exposed one active false negative:

```powershell
.\gradlew.bat --no-daemon testBackendSlow
```

It failed after 7m 31s in `test_reported_long_cs_mrr_regression`. The new
equal-endpoint-sign certificate rejected a valid generalized sine boundary
edge between approximately `(2.59642, 0.457958)` and
`(2.59796, 0.456321)`. Numerical diagnosis found the tested curve remained
strictly positive on the nearby implicit boundary arc; the failure was
interval dependency growth, not evidence of a concealed crossing.

`test_long_cs_positive_generalized_edge_is_certified` now isolates the exact
52-term curve, 46-term boundary, and reported endpoints in
`intersection_test.hpp`. Before the repair it failed in 122286 microseconds.

The sine/sine hidden-crossing certificate now tries algebraically equivalent
curves formed by adding or subtracting the boundary equation before falling
back to the original expression. The boundary is exactly zero on the
candidate arc, so this transformation cannot alter the curve's values or
zeros there. It only cancels shared symbolic terms before interval
evaluation. In this reproducer, addition reduces the expression from 52 terms
to 33 and removes the dependency blow-up while retaining the same
fail-closed proof.

Fresh evidence after the repair:

```powershell
.\gradlew.bat --no-daemon testExecutable
$env:PATH = 'C:\msys64\ucrt64\bin;' + $env:PATH
.\build\exe\test\test.exe --run_test=test_long_cs_positive_generalized_edge_is_certified --log_level=test_suite
```

`testExecutable` rebuilt successfully in 1m 9s. The isolated regression then
passed with `*** No errors detected` in 1147 microseconds, approximately 107
times faster than the failing certificate.

This is an intermediate checkpoint only. The full fast native suite and
`testBackendSlow` still must pass with the repair before the native critical
rows can be closed.

The subsequent fast native gate passed all 45 cases:

```powershell
.\gradlew.bat --no-daemon testBackend
```

Result: `BUILD SUCCESSFUL in 7s`, `*** No errors detected`.

The next `testBackendSlow` attempt did not complete within the command's
15-minute limit. The command wrapper exited 124 after 904 seconds with the
native test child still running; that exact campaign-owned child was stopped
and its Gradle processes then exited. This is not a test failure or a pass,
but it is a material performance regression: the same long workload
previously benchmarked in seconds. Diagnose and remove the certificate hot
path before rerunning the slow gate. Do not merely raise the timeout and do
not mark the affected bugs fixed.

### Exact continuation

1. Profile or instrument the hidden-crossing certificate on one
   `reported-long-cs-mrr` worker run and identify the expensive proof calls.
2. Preserve fail-closed correctness while restoring practical runtime; keep
   the isolated positive-edge regression and hidden-two-root rejection.
3. Rerun `testBackend`, then `testBackendSlow`; do not accept the repair
   unless the complete 1-worker and 4-worker long regression passes.
4. Record both results here and in the temp workstate immediately.
5. Run the documented 1/2/4-worker, five-sample benchmark and compare every
   digest.
6. Run handbook coverage enforcement/build, close supported active critical
   tracker rows, and finish release/handbook documentation.
7. Preserve the five pre-existing dirty paths, remove only campaign-generated
   cache changes, and do not commit or push without authorization.

## Long-workload hot-path checkpoint 6 - 2026-07-28

Profiling the bounded one-worker `reported-long-cs-mrr` workload found that
the remaining stall was another interval-dependency case, this time on a
Cos/Cos generalized edge. The curve shared its first 24 terms with the
34-term boundary; direct implicit-arc subdivision exceeded 255 pieces even
though subtracting the boundary leaves only the short differing tail.

The exact Cos/Cos hidden-crossing overload now tries the curve plus and minus
the zero-valued boundary before the pre-existing cosine factorization and
adaptive fallback. This transformation is rigorous on the boundary's zero
set, is accepted only when it reduces the symbolic term count, and remains
fail-closed when the quick interval certificates cannot prove a strict sign
or monotonicity.

Fresh evidence:

```powershell
.\gradlew.bat --no-daemon testExecutable
```

rebuilt successfully in 1m 8s. The bounded one-worker benchmark then
completed with `*** No errors detected` in 12.217235 seconds rather than
exceeding the prior 15-minute limit:

```text
BENCH_RESULT hash=9f837d3bd0c612a7 points=7 equations=7 workers=1
```

The temporary 256-piece diagnostic tripwire was removed after this run. No
temporary diagnostic code remains in `intersection.hpp`.

This is a performance-recovery checkpoint, not final acceptance. The new hash
differs from the pre-campaign documented `883e8b0c1317184e`, plausibly because
the active native correctness repairs change which certified boundary pieces
survive, but that difference has not yet been audited. Do not overwrite the
old benchmark record or mark the affected bugs fixed until the digest change,
fast native suite, complete 1/4-worker slow regression, and worker-parity
benchmark have all been checked.

### Exact continuation

1. Rebuild after diagnostic removal and run the focused intersection
   regressions plus the full `testBackend` suite.
2. Audit the old/new long-workload boundary digest difference and establish
   whether it is the intended result of the correctness repairs.
3. Run `testBackendSlow`; require both one- and four-worker calculations to
   complete and compare equal.
4. Immediately append the results here and to
   `C:\tmp\abdul-repair-workstate-2026-07-27.md`.
5. Run the five-sample 1/2/4-worker benchmark and require one digest across
   all samples.
6. Complete Abdul-comment hygiene, handbook enforcement/build, tracker and
   release documentation, then remove only campaign-generated cache changes.
   Preserve the five pre-existing dirty paths and do not commit or push.

## Fast-gate and digest-audit checkpoint 7 - 2026-07-28

The first fast native gate after checkpoint 6 exposed a distinct active
certificate regression in `test_calculate_empty`. A straight boundary
`5x+2y-7eta` and cosine curve had one simple root on the tested edge, but the
univariate derivative also vanished at the non-root endpoint
`(pi/2, pi/2)`. Requiring a nonzero derivative over the complete edge failed,
while proving the curve nonzero on intervals ending only about `1e-30` from
the certified root exceeded the adaptive resolution.

The straight-edge uniqueness proof now:

1. retains the whole-edge monotonicity fast path;
2. searches for a derivative-nonzero neighborhood containing the complete
   certified root interval; and
3. proves the curve nonzero on both exterior parameter intervals.

This is a rigorous decomposition: the Krawczyk box supplies the existing
root, strict derivative sign supplies at most one root in its neighborhood,
and strict nonzero exterior intervals rule out every other edge root.
`test_line_cosine_unique_root_allows_endpoint_derivative_zero` isolates the
exact line, curve, and symbolic endpoints. It passed in 5.473 ms.

The focused cohort also retained:

- rejection of a line/cosine edge hiding two roots;
- the long Sin/Sin positive-edge certificate; and
- the new long Cos/Cos shared-term certificate.

All four focused cases passed with `*** No errors detected`. The complete
fast native gate then passed 47 cases:

```powershell
.\gradlew.bat --no-daemon testBackend
```

Result: `BUILD SUCCESSFUL in 7s`, `*** No errors detected`.

### Old/new digest audit

A clean source snapshot of exact HEAD
`0d2be2093bed5b4673b855123f1ff8ce16d2fa20` was created under the ignored
`tmp/head-digest-audit-20260728/` tree and built independently with Gradle 8,
JDK 17, and offline cached dependencies. Its bounded one-worker workload
reproduced the documented baseline:

```text
BENCH_RESULT hash=883e8b0c1317184e points=7 equations=7 workers=1
```

The repaired tree produced:

```text
BENCH_RESULT hash=9f837d3bd0c612a7 points=7 equations=7 workers=1
```

An opt-in `BILLIARDS_DUMP_BENCHMARK_BOUNDARY` payload dump then compared the
complete normalized hash inputs. Exact findings:

- initial angles are identical;
- both results contain 14 coordinate intervals for 7 points;
- all seven serialized equations are byte-for-byte identical;
- all seven serialized `LeftRight` provenance records are byte-for-byte
  identical;
- only the 14 coordinate intervals differ;
- the old Krawczyk predecessor boxes have approximately `1e-25` half-width,
  while the new strict Krawczyk boxes have approximately `1e-30` half-width
  around the same roots.

The hash change is therefore an intended certificate-precision change, not a
lost point, equation, boundary, or provenance record. Future post-repair
worker-parity checks should require `9f837d3bd0c612a7`; retain the old hash in
historical baseline prose and label it as pre-repair evidence.

### Exact continuation

1. Run complete `testBackendSlow` and require both one- and four-worker
   calculations to finish with identical full boundaries.
2. Checkpoint that evidence immediately in both durable workstate files.
3. Run the documented five-sample benchmark at 1, 2, and 4 workers; require
   `9f837d3bd0c612a7` for every sample and compare timing distributions.
4. Audit all handwritten hunks for exact dated Abdul comments.
5. Run handbook coverage enforcement/build, finish tracker and release docs,
   remove the isolated HEAD audit tree and only campaign-generated caches,
   preserve the five pre-existing dirty paths, and do not commit or push.

## Complete slow-native gate checkpoint 8 - 2026-07-28

The complete post-repair slow native gate passed:

```powershell
.\gradlew.bat --no-daemon testBackendSlow
```

Exact result:

- `BUILD SUCCESSFUL in 30s`;
- 47 native test cases;
- `*** No errors detected`;
- the exact reported 124-number CS workload completed first at one worker and
  then at four workers;
- `check_same_stable_boundary` confirmed identical initial angles, equations,
  `LeftRight` provenance, point counts, and every interval endpoint.

The build and test executable were already current, so the Gradle task
executed only the native slow runner. Its expected malformed-gradient
diagnostic (`unable to parse equation: not-an-equation`) was emitted by the
negative wrapper regression and did not represent a failure.

This resolves the slow-gate failure/stall documented in checkpoint 5:

- initial run: false negative after 7m31s;
- next diagnostic run: exceeded 15 minutes;
- repaired full one/four-worker gate: pass in 30s.

### Exact continuation

1. Run the documented five-sample `reported-long-cs-mrr` benchmark at 1, 2,
   and 4 workers; require hash `9f837d3bd0c612a7` for all 15 measured samples
   and compare timings with the historical baseline.
2. Checkpoint raw evidence and summary immediately.
3. Audit every handwritten code/test/build/script hunk for a nearby exact
   `abdul dd/mm/yyyy [reason]` comment.
4. Run handbook coverage enforcement and reader builds; repair the coverage
   ledger and documentation.
5. Update every supported critical bug row and release narrative. Keep
   `BUG-166`/`BUG-168` open and dormant.
6. Remove the ignored HEAD audit tree and campaign-generated caches only,
   preserve the five pre-existing dirty paths, run final clean gates/diff
   hygiene, and do not commit or push.

## Worker-parity benchmark checkpoint 9 - 2026-07-28

The documented benchmark runner completed one warmup plus five measured
samples at each of 1, 2, and 4 workers:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command `
  "& { & '.\tools\benchmark\run-benchmarks.ps1' `
  -Workload 'reported-long-cs-mrr' -Samples 5 `
  -WorkerCounts @(1,2,4) -SkipBuild }"
```

The runner's own build subprocess encountered the already documented
automation-host Gradle-wrapper cache mismatch before sampling. `-SkipBuild`
was therefore used and is preserved honestly in metadata as
`baseline-unverified-skip-build`. The exact executable had separately been
rebuilt successfully immediately beforehand and then passed both the
47-case `testBackend` and 47-case `testBackendSlow` gates; the skip-build
label does not replace or obscure those build/test records.

Evidence directory:

`build/benchmarks/20260728-043126-reported-long-cs-mrr-baseline-unverified-skip-build/`

Results:

| Workers | Valid samples | Median wall | Range | Median CPU | Max working set | Hash |
| ---: | ---: | ---: | ---: | ---: | ---: | --- |
| 1 | 5/5 | 12,625.641 ms | 12,570.253-12,681.309 ms | 12,453.125 ms | 41,857,024 B | `9f837d3bd0c612a7` |
| 2 | 5/5 | 10,404.791 ms | 10,280.089-10,497.297 ms | 13,015.625 ms | 49,147,904 B | `9f837d3bd0c612a7` |
| 4 | 5/5 | 9,380.152 ms | 9,309.384-9,412.134 ms | 13,484.375 ms | 49,590,272 B | `9f837d3bd0c612a7` |

All 15 measured samples were valid, correctness failures were zero, and the
runner found exactly one result hash. The three warmups were also valid and
had the same hash.

The historical pre-repair medians were 9,897.271, 6,051.055, and
4,342.074 ms at 1, 2, and 4 workers. The new proof-carrying path is therefore
about 27.6%, 71.9%, and 116.0% slower in this whole-workload comparison.
That is a real performance cost and must not be presented as an optimization
win. The near-flat CPU totals and weaker worker speedup are consistent with
more rigorous sequential intersection/crossing certification after parallel
curve refinement. The repaired path remains practical (complete slow gate in
30s rather than greater than 15 minutes), deterministic, and materially safer
than the pre-repair heuristic.

### Exact continuation

1. Perform the exact Abdul-comment and diff-hygiene audit across every
   handwritten code/test/build/script hunk.
2. Run handbook coverage enforcement and rebuild the reader artifacts using
   the documented commands.
3. Update the live bug register, current handbook, release README,
   significant changes, and final repair report from checkpoints 1-9.
   Record the benchmark slowdown and create/retain an optimization follow-up
   rather than claiming faster throughput.
4. Keep `BUG-166`/`BUG-168` open and dormant.
5. Remove the ignored HEAD audit tree and only campaign-generated caches,
   preserve the five pre-existing dirty paths, run final clean verification
   and `git diff --check`, and do not commit or push.

## Handbook and release-accounting checkpoint 10 - 2026-07-28

The source inventory was synchronized after the repair. Whole-file
fingerprints invalidated 1,111 previously reviewed/grouped symbols, and the
new production surface introduced 267 rows. Handbook Sections 14.182 through
14.189 now explain:

- exact corner-gradient recovery and checked normalization;
- strict Krawczyk common-root, connected-arc, unique-crossing, and
  hidden-crossing certificates;
- application-owned operation generations and terminal publication;
- immutable Vary/Tetra/Super requests and typed reconstructed validation;
- typed Cover triples and staged manifest-backed artifact replacement;
- complete render snapshots and committed camera/raster pairs;
- staged Save V3 output and the five updater deletions; and
- the exact digest audit plus measured performance cost.

Existing rows were restored only after affected-file review. New
behavior-bearing symbols received exact explanatory entry markers; local
fields, constants, enum values, closures, and parameter-piece carriers were
grouped with written rationales under their owning algorithm. The
Doxygen-enabled enforcing audit passed:

```text
Inventory symbols: 4435
Production symbols: 4302
Reviewed: 2942
Grouped trivial: 1360
Excluded support-test rows: 133
Explanatory gaps: 0
Coverage: 100%
```

Doxygen retained 25 known nonfatal template/member-matching warnings. The
reader builder then completed with:

```text
HTML: build/handbook/CODEBASE-HANDBOOK.html (2,400,484 bytes)
PDF:  build/handbook/CODEBASE-HANDBOOK.pdf  (11,021,461 bytes)
HTML MathML nodes: 421
```

The initial sandboxed Headless Chrome invocation returned without writing the
PDF; the same documented builder was rerun with the required browser
permission and exited zero. Generated reader/audit files remain ignored.

The final integrity pass also found that 55 Ctags rows occupied only 20
apparent IDs, so the old hash maps silently collapsed 35 real ledger symbols.
`DOC-004` now assigns stable source-order ordinal suffixes within each
collision group and makes duplicate inventory or ledger IDs fatal. Both
generated inventory and source ledger contain 4,435 rows and 4,435 unique IDs;
the Doxygen-enabled 4,302/4,302 production gate still passes.

The live register now contains 323 unique IDs. All 23 in-scope active critical
rows are `fixed` with exact implementation and validation evidence.
`BUG-166` and `BUG-168` remain `open` and explicitly dormant. `OPT-027`
records the measured post-repair certification slowdown and the proof
contracts that any optimization must preserve. `DOC-002` records the new
reader/audit evidence, and `DOC-004` records the duplicate-ID repair.

Release-facing documentation updated:

- `docs/CODEBASE-HANDBOOK.md`;
- `docs/handbook/coverage.csv`;
- `docs/handbook/HANDBOOK-WORK-STATE.md`;
- `docs/codex-project-study/bug-register.csv`;
- `docs/release/README.md`;
- `docs/release/ABDUL-WINDOWS-RELEASE-REPORT.md`;
- `docs/release/SIGNIFICANT-APPLICATION-CHANGES.md`; and
- `docs/release/EVIDENCE-FIRST-REPAIRS-2026-07-27.md`.

### Exact continuation

1. Remove only the ignored independent-HEAD audit tree
   `tmp/head-digest-audit-20260728` after resolving and verifying that exact
   target is within this repository's `tmp` directory.
2. Restore only campaign-generated tracked Gradle cache paths to their HEAD
   bytes while preserving the pre-campaign dirty
   `.gradle/8.0/executionHistory/executionHistory.lock`.
3. Run the final clean Java/native/fast gate, then the complete slow native
   gate if practical after the clean rebuild.
4. Re-run handbook enforcement if any source changes, run CSV/reference checks
   and `git diff --check`, and inspect final status for accidental artifacts.
5. Append final checkpoint 11 here and to the temp workstate. Preserve
   `cover/info.txt`, `garbage.txt`, `tmp/cover_magnifications.txt`,
   `tmp/cover_stables.txt`, and the execution-history lock. Do not commit or
   push.

## Final validation and completion checkpoint 11 - 2026-07-28

This checkpoint completes the user-approved plan. It supersedes the
continuation lists above without erasing their chronological evidence.

### Scope outcome

- All 23 active critical rows in the campaign are `fixed`:
  `BUG-034`, `BUG-037`, `BUG-038`, `BUG-039`, `BUG-177`, `BUG-178`,
  `BUG-181`, `BUG-185`, `BUG-186`, `BUG-187`, `BUG-188`, `BUG-192`,
  `BUG-202`, `BUG-203`, `BUG-205`, `BUG-212`, `BUG-216`, `BUG-217`,
  `BUG-219`, `BUG-220`, `BUG-229`, `BUG-231`, and `BUG-243`.
- `BUG-166` and `BUG-168` remain `open` and dormant exactly as directed.
- `OPT-027` remains `open`: strict hidden-crossing certification is correct
  and deterministic, but its sequential cost is a real optimization target.
- The live register has 323 rows and 323 unique IDs. The final Markdown
  cross-reference audit found 309 distinct register IDs and zero unknown
  references.

### Final build and test gates

The final clean gate ran:

```powershell
.\gradlew.bat --no-daemon clean compileJava backendSharedLibrary test testBackend
```

It completed successfully in 2 minutes 47 seconds with 12 executed tasks,
28 JUnit tests, 47 native cases, and zero failures, errors, or skips. The
complete slow gate then ran:

```powershell
.\gradlew.bat --no-daemon testBackendSlow
```

It passed all 47 cases in 29 seconds with `*** No errors detected`, including
the exact reported long-CS workload at one and four workers and full boundary
equality. After diff/line-ending hygiene and handbook-ledger synchronization,
`compileJava backendSharedLibrary` passed again in 1 minute 22 seconds. The
nine compiler warnings were the same known nonfatal warnings documented
earlier; no new failure was suppressed.

`C:\msys64\usr\bin\bash.exe -n package-mac.sh` exited zero. Source and
packaging scans found no Java updater class reference and no executable
reference to the deleted updater filenames. The Windows and macOS packaging
scripts retain only dated explanatory comments for the removal.

### Retained benchmark evidence

The final clean gate correctly removed ignored `build/` products, including
the first raw matrix recorded in checkpoint 9. To avoid losing raw evidence,
the already rebuilt and fully tested executable was sampled again with:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command `
  "& { & '.\tools\benchmark\run-benchmarks.ps1' `
  -Workload 'reported-long-cs-mrr' -Samples 5 `
  -WorkerCounts @(1,2,4) -SkipBuild }"
```

The retained directory is:

`build/benchmarks/20260728-052151-reported-long-cs-mrr-baseline-unverified-skip-build/`

It contains 41 files: metadata, Markdown/CSV/JSON summaries, the 18-row sample
table, and every warmup/sample stdout and stderr stream. All 15 measured
samples and three warmups exited zero, were valid, and produced the single
normalized result hash `9f837d3bd0c612a7`.

| Workers | Valid samples | Median wall | Range | Median CPU | Max working set | Change from historical |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 1 | 5/5 | 12,644.050 ms | 12,564.390-12,776.184 ms | 12,468.750 ms | 41,857,024 B | +27.8% |
| 2 | 5/5 | 10,573.877 ms | 10,472.016-10,627.669 ms | 13,281.250 ms | 49,119,232 B | +74.7% |
| 4 | 5/5 | 9,476.218 ms | 9,408.298-9,488.579 ms | 13,328.125 ms | 49,537,024 B | +118.2% |

The comparison medians remain 9,897.271, 6,051.055, and 4,342.074 ms.
Checkpoint 9 remains an accurate record of the first sampling run; the table
above is authoritative because its complete raw directory is retained. This
directory is intentionally ignored build evidence and a future Gradle
`clean` will remove it. Do not run another clean before deliberately copying
or archiving that exact directory if the raw streams must outlive this
working tree.

### Documentation and integrity gates

- Doxygen-enabled handbook enforcement passed 4,302/4,302 production symbols:
  2,942 reviewed plus 1,360 justified grouped-trivial rows, with 133
  support-test rows excluded and zero gaps.
- Inventory and ledger each contain 4,435 rows and 4,435 unique IDs.
  `DOC-004` makes future duplicate inventory or ledger identities fatal.
- The final reader build produced
  `build/handbook/CODEBASE-HANDBOOK.html` at 2,400,967 bytes with 421 MathML
  nodes and `build/handbook/CODEBASE-HANDBOOK.pdf` at 11,022,806 bytes with a
  valid `%PDF-` signature.
- `git diff --check` exited zero. Git emitted only its expected advisory that
  configured checkout line endings may become CRLF when Git next rewrites
  particular files.
- The changed handwritten production/test/build/script audit covered 59 files;
  all 59 contain the required exact 27/07/2026 or 28/07/2026 Abdul marker.
- The independent normalized payload audit remains exact: historical HEAD
  hash `883e8b0c1317184e`, repaired hash `9f837d3bd0c612a7`, identical initial
  angles/equations/`LeftRight` records/root centers, and only the 14 interval
  endpoints tightened to strict Krawczyk boxes.

### Preservation and continuation boundary

The independent-HEAD diagnostic tree is absent. Only the pre-existing
`.gradle/8.0/executionHistory/executionHistory.lock` remains dirty below
`.gradle`; all campaign-generated tracked Gradle cache changes were restored
to their exact HEAD bytes. The five protected pre-campaign paths remain dirty:

- `.gradle/8.0/executionHistory/executionHistory.lock`;
- `cover/info.txt`;
- `garbage.txt`;
- `tmp/cover_magnifications.txt`; and
- `tmp/cover_stables.txt`.

Runtime defaults remain `2g` initial heap, `6g` maximum heap, and `2g` direct
memory in `build.gradle`. No database was moved, no installer was run, and no
commit, push, tag, release, or external publication was performed.

There is no unfinished item in the approved repair plan. A future agent
should resume only if the user opens new scope. Sensible optional follow-ups
are manual JavaFX workflow smoke tests on a display-capable Windows session
and `OPT-027` profiling/optimization under the proof-preservation constraints.
Do not activate dormant Small Cover or run a new clean build merely to repeat
this completed checkpoint.
