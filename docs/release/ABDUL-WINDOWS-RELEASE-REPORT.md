# Abdul Windows Release Report

Date: 2026-07-14; latest critical-repair addendum: 2026-07-28

Primary source tree: Abdul Windows,
`Abdul-Windows-fork-BilliardsEverything`

Comparison names used in this report:

- Abdul Windows: this active Windows source tree.
- Nick Shan Linux: the Linux port and `Project_Improvement_Report.md`.
- Suryansh Mac: the older Mac/runtime line and build assumptions reflected in the source/build files.
- Main runtime: the built Windows runtime/jar/backend artifacts studied earlier. Treat as runtime evidence, not source truth.

This report is meant to ship with the test build. It explains what changed, why it changed, what risk remains, and how volunteers should run the program.

## Executive Summary

The Abdul Windows tree now builds the JavaFX frontend and native C++ backend through Gradle on Windows/MSYS2 UCRT64. The release work focused on correctness, memory pressure, cancellation safety, Windows build reliability, and making the UI usable enough for volunteer testing.

The most important release changes are:

- Fixed Windows native build/link issues for MSYS2/UCRT64.
- Targeted Java 17 explicitly.
- Preserved the intended runtime memory settings: `-Xms2g`, `-Xmx6g`, `MaxDirectMemorySize=2g`.
- Limited Gradle compile pressure with `org.gradle.workers.max=2`.
- Added a user-facing app thread argument, `--threads=N`.
- Pushed the resolved Java thread count into the native backend.
- Fixed the long-MRR boundary-provenance failure reported as `this isn't supposed to happen-x-y+2eta` while retaining safe internal MRR parallelism.
- Fixed AutoPolyVary forking multiple task chains when a prior result covered the next coordinate.
- Stopped native calculation failures from being cached and printed as mathematical empty sets.
- Fixed the reflected viewer being shifted vertically by pinning the image stack to the actual 600x600 image size.
- Added an `Add to Cover` checkbox next to the existing top Calculate button.
- Added duplicate guards so that Calculate does not insert the same cover line twice.
- Ported or independently implemented the highest-value Nick Shan Linux fixes: numeric tolerance, Vary4 geometry typo, stale cover guard, pixel bounds guard, evaluator reuse, render batching/coalescing, DB connection ownership, code canonicalization, and pattern-finder string building.
- Retained Krawczyk, connected-arc, unique-crossing, and hidden-crossing machinery as focused proof research without making it a production gate over established MRR inputs.
- Added application-owned operation generations that cancel and join active Viewer workflows before native-pool destruction.
- Added immutable Vary/Tetra/Super requests, typed reconstructed-storage validation, staged cover publication, and committed camera/raster rendering.
- Disabled and removed the unauthenticated automatic updater.
- Deferred database relocation because changing user data paths needs a deliberate migration plan.

## 2026-07-28 Critical Repair Release State

The active critical campaign is documented line by line in
`CRITICAL-REPAIR-CHECKPOINT-2026-07-27.md`; the maintainable source contracts
are handbook Sections 14.182 through 14.189. The live bug register marks every
in-scope active critical row fixed after implementation and focused evidence.
`BUG-166` and `BUG-168` remain open because the user explicitly declared Small
Cover dormant. They were neither activated nor silently relabeled.

This campaign introduced Krawczyk, connected-component, and concealed-crossing
proof machinery and measured its behavior. The 31 July `nextIter` regression
showed that promoting it to the production acceptance path rejected established
MRR inputs. Production now uses the compatible full/half-segment Newton search
with exact two-equation zero validation; the stricter machinery remains in the
source for focused proof research and tests.

The Java application now owns asynchronous work through one
`OperationRegistry`. Exclusive workflow keys prevent overlapping VaryL,
PolyVary, and Tetra/Bar generations. Futures and executors are registered
before publication, terminal transitions are exactly once, queued JavaFX
callbacks recheck the generation, and application shutdown stops admission,
cancels, and joins workers before destroying the native pool.

Search controls are snapshotted into immutable requests before background
execution. SuperPolyVary settings use a complete versioned atomic file. The
Tetra/Bar accumulator is generation-private. One-point VaryL no longer writes
the multi-point coordinate file or reuses a stale result. Reconstructed
storage validation distinguishes proved valid, proved invalid, and
inconclusive evidence; only proved invalid permits database deletion.

Cover triple input is structurally stable-unstable-stable. Cover merge writes
and validates one sibling staging generation, records a SHA-256/size manifest,
retains rollback, and switches live under a shared artifact lock. Rendering
captures every camera/control/geometry input on JavaFX and commits the images
with that copied camera and render generation. Hole scans and automatic
sampling fail closed without a matching committed pair.

The old updater could not meet authenticity or transactional installation
requirements. The UI is disabled with manual trusted-install guidance, and
`Updater.java` plus both root/app platform-script pairs were deleted. A future
updater requires a new signed-manifest and rollback design. The Windows and
macOS packagers no longer copy the deleted scripts, so installer construction
does not fail or silently restore the removed payload.

Validation completed before release documentation:

- initial clean `compileJava backendSharedLibrary test testBackend`: 28 JUnit
  tests and 44 then-current native cases, zero failures;
- final clean gate after the new intersection fixtures: 28 JUnit tests and 47
  native cases in 2 minutes 47 seconds, zero failures;
- `testBackendSlow`: 47 cases and exact one/four-worker long-boundary
  equality in 29 seconds;
- five measured samples each at one, two, and four workers: one repaired hash
  `9f837d3bd0c612a7`, zero correctness failures;
- retained raw benchmark evidence:
  `build/benchmarks/20260728-052151-reported-long-cs-mrr-baseline-unverified-skip-build/`;
- Doxygen-enabled handbook enforcement: 4,310/4,310 production symbols;
- unique handbook identity check: 4,453/4,453 inventory/ledger IDs after
  `DOC-004` disambiguated explicit specializations;
- final reader build: 2,400,967-byte embedded HTML and 11,022,806-byte PDF
  with 421 MathML nodes.

The strict proof path is slower than the historical heuristic: median wall
time changed by +27.8%, +74.7%, and +118.2% at one, two, and four workers.
This is tracked openly as `OPT-027`. Optimizations must preserve strict proof
contracts and the repaired normalized payload; the slowdown is not described
as a speed win.

## Build And Launch Summary

See `BUILD-RUN-GUIDE.md` for full instructions.

Windows dependency install:

```powershell
C:\msys64\usr\bin\pacman.exe -S --needed `
    mingw-w64-ucrt-x86_64-gcc `
    mingw-w64-ucrt-x86_64-boost `
    mingw-w64-ucrt-x86_64-eigen3 `
    mingw-w64-ucrt-x86_64-tbb `
    mingw-w64-ucrt-x86_64-gmp `
    mingw-w64-ucrt-x86_64-mpfr `
    mingw-w64-ucrt-x86_64-mpfi `
    mingw-w64-ucrt-x86_64-sqlite3 `
    mingw-w64-ucrt-x86_64-gdb `
    mingw-w64-ucrt-x86_64-pkgconf `
    mingw-w64-ucrt-x86_64-make
```

Build and run:

```powershell
.\gradlew.bat --no-daemon run
```

Build and run with a worker limit:

```powershell
.\gradlew.bat --no-daemon run --args="--threads=2"
```

Validation commands used during this release pass:

```powershell
.\gradlew.bat --no-daemon compileJava backendSharedLibrary
.\gradlew.bat --no-daemon testBackend
.\gradlew.bat --no-daemon test
.\gradlew.bat --no-daemon testBackendSlow
```

## Change Register

### BUG-001: Native Numeric Convergence

Files:

- `src/backend/headers/newton.hpp`
- `src/backend/headers/intersection.hpp`

Bug:

Abdul Windows used extremely tight numeric thresholds around Newton solving and intersection checks. On long OSNO and cover paths this risks false non-convergence or intervals that are too brittle for practical MPFR/MPFI work.

Fix:

Changed the tolerance/fudge values to `1e-25`, matching the Nick Shan Linux evidence.

Philosophy:

This code needs certified-enough interval behavior in the real application, not a tolerance so tight that valid long inputs become unstable. The fix follows the Linux port because it was already tested against large cases.

Conditions:

Most relevant when calculating long OSNO sequences, covers, MRRs, and recursive stable/unstable checks.

Significance:

High correctness risk. A bad tolerance can produce wrong empty/non-empty or convergence behavior.

Validation:

`compileJava backendSharedLibrary` passed. Long manual OSNO comparison is still recommended.

### BUG-002: Vary4 Right-List Geometry Typo

File:

- `src/backend/cpp/triangle_billiard4.cpp`

Bug:

The `reconfigure(false)` branch looped over the right-side candidate list but indexed the left-side list. That can produce wrong geometry or an out-of-bounds read if the lists differ.

Fix:

Changed the right-side branch to index `R[i]`, matching Nick Shan Linux.

Philosophy:

This is a direct correctness bug, not an optimization. The loop variable and source list must match.

Conditions:

Vary4 calculations that generate unequal left/right candidate lists.

Significance:

High. Wrong candidate geometry can poison later vary output.

Validation:

Native backend compiled. Manual Vary4 cases with unequal candidate lists should be tested.

### BUG-003: Stale Cover Artifact Crash

File:

- `src/java/billiards/cover/CoverStuff.java`

Bug:

`cover.txt` stores indexes into companion `stables.txt` and `triples.txt`. If files are stale or manually edited, an index can point past the available list and crash load.

Fix:

Added bounds checks. Bad entries warn and skip instead of crashing the viewer.

Philosophy:

Runtime artifacts are user-editable and often carried between versions. Loading should be defensive.

Conditions:

Opening old, partial, hand-edited, or version-mismatched cover folders.

Significance:

Medium stability. Prevents volunteer tests from failing on stale artifacts.

Validation:

Compile passed. Manual stale-cover load test still recommended.

### BUG-004: AutoVary Pixel Bounds

Files:

- `src/java/billiards/viewer/PolyVaryTask.java`
- `src/java/billiards/viewer/CycleVaryTask.java`

Bug:

Pixel reads could happen outside image bounds after zoom, reflection, or edge rounding.

Fix:

Added image-null and pixel-bound checks. Interrupted pixel probes preserve interrupt state.

Philosophy:

AutoVary should skip impossible screen probes rather than crash an entire search.

Conditions:

Zoomed/reflected views, edge pixels, and cancellation.

Significance:

Medium stability.

Validation:

Compile passed. Manual AutoPolyVary and CycleVary edge tests remain important.

### BUG-005: Interrupt And Executor Lifecycle Audit

Status:

Completed for active Viewer workflows on 2026-07-28.

Files:

- `src/java/billiards/viewer/*.java`

Issue:

Java tasks needed one owner for interrupted waits, executor shutdown, cancel
timing, terminal publication, and partial-progress preservation.

Fix:

`OperationRegistry` now owns application admission, exclusive workflow
generations, Futures, and per-operation executors. Active Viewer workflows
route every terminal path through an exactly-once handle and reject late
JavaFX publication. `IterateToLimitWindow` closes its synchronous pool from
`finally`, restores interrupts, and cancels siblings. Main joins registered
operations before native-pool destruction.

Scope:

The source scan classified Boyan, Cover, Cycle, lookup, Tetra/Bar, VaryL,
direct/Auto/Super PolyVary, code loading, merge, and render resources.
PatternFinder is a separate program selection and was not folded into the
Viewer lifecycle.

Validation:

Three registry tests cover overlap, executor-retirement ownership, and
shutdown cancellation/join. The 27-test forced critical cohort and clean
28-test gate passed. Manual close-during-work tests remain useful for JavaFX
interaction behavior, but no known active Viewer ownership gap remains.

### BUG-006: Viewer Shifted Down

File:

- `src/java/billiards/viewer/Viewer.java`

Bug:

The reflected viewer could appear shifted down because `updateReflection()` translated by `imageStack.getBoundsInLocal().getHeight()`. The stack lives in a `BorderPane` center and can receive more vertical layout height than the actual 600x600 image.

Fix:

Pinned `imageStack` min/pref/max size to `SIDE x SIDE` and changed the reflection translation to `SIDE`.

Philosophy:

The reflection transform should flip the drawing surface, not the parent layout area. The map and image pixels are defined in `0..SIDE`, so the transform should also use `SIDE`.

Conditions:

Main viewer with Reflect selected, especially in windows taller than the 600px image.

Significance:

High for volunteer usability. If the viewer is visibly displaced, users will distrust the build.

Validation:

Compile passed. Manual launch required: verify the viewer is centered and stable, then toggle Reflect repeatedly.

### FEAT-001: Top Calculate Add-To-Cover Checkbox

Files:

- `src/java/billiards/viewer/Viewer.java`
- `src/java/billiards/viewer/CoverWindow.java`

Feature:

Added `Add to Cover` next to the existing top Calculate button. This does not add a second Calculate button or a new polygon box.

Behavior:

- Existing Calculate still calculates and draws as before.
- If `Add to Cover` is checked, Calculate also tries to append the result to the Cover window.
- Stable singles are appended to the stable cover text.
- Stable-unstable-stable triples are appended to the triple cover text.
- Unstable singles are skipped because the Cover window does not directly store them as stable cover entries.
- Empty or incomplete results are skipped.
- Exact duplicate lines are skipped before append.

Philosophy:

This mirrors the useful cover-writing behavior of the Li pattern/iteration flow, but keeps the main UI simple: one checkbox, one existing Calculate action.

Conditions:

Use when a volunteer calculates a known stable or triple and wants it in the Cover window immediately.

Significance:

Medium release usability. Reduces copy/paste errors and makes testing cover workflows easier.

Validation:

Compile passed. Manual tests:

1. Check `Add to Cover`.
2. Calculate a stable single twice.
3. Verify the Cover window receives one line, not two.
4. Calculate a valid stable-unstable-stable triple twice.
5. Verify the triple area receives one line, not two.

### OPT-001: Cover Info Annotation

File:

- `src/java/billiards/viewer/CoverWindow.java`

Issue:

`redoInfo` rescanned all pre-info lines for every info line.

Fix:

Builds a lookup map once, making the operation O(n+m).

Philosophy:

Large cover folders can contain thousands of lines. Repeated full scans are unnecessary and make UI tasks feel frozen.

Significance:

Medium performance.

Validation:

Compile passed. Manual large cover output comparison recommended.

### OPT-002: Code Sequence Canonicalization

Files:

- `src/backend/cpp/code_sequence.cpp`
- `src/backend/headers/code_sequence.hpp`
- `src/test/headers/code_sequence_test.hpp`

Issue:

Canonicalizing rotations/reversals used an O(n^2)-style rotate/compare scan.

Fix:

Added least-rotation logic and `rotated_copy`, then compares forward and reversed representatives in O(n). Also caches `CodeSequence::type()` because code sequences are immutable after construction.

Philosophy:

This is core math plumbing. It must preserve canonical equivalence while reducing cost on long code sequences.

Conditions:

Long code sequence parsing, classification, vary, and database lookup.

Significance:

Medium performance, high confidence due native regression test.

Validation:

`testBackend` ran 31 Boost tests with no errors.

### OPT-003: Cover Evaluator Allocation

Files:

- `src/backend/cpp/evaluator.cpp`
- `src/backend/headers/evaluator.hpp`
- `src/backend/cpp/common.cpp`
- `src/backend/cpp/verify.cpp`

Issue:

Hot cover paths repeatedly constructed MPFR/MPFI evaluator state.

Fix:

Added thread-local evaluator reuse per worker thread and precision.

Philosophy:

MPFR/MPFI allocation is expensive and mostly scratch-state. Reuse reduces allocation churn while keeping thread isolation.

Conditions:

Cover recursion, MRR, long OSNO cover work.

Significance:

High memory/performance.

Validation:

Compile passed. Manual memory plateau test still required.

### OPT-004: AutoVary Pixel Reader Caching

Files:

- `src/java/billiards/viewer/PolyVaryTask.java`
- `src/java/billiards/viewer/CycleVaryTask.java`

Issue:

Pixel reads were serialized through repeated JavaFX calls and traversal was shuffled.

Fix:

Snapshot `PixelReader` and dimensions once, remove per-coordinate JavaFX round trips, and keep deterministic traversal.

Philosophy:

Background search should not depend on thousands of tiny UI-thread calls.

Significance:

Medium performance and stability.

Validation:

Compile passed. Manual AutoVary/CycleVary cancellation and edge tests remain.

### OPT-005: Render Pipeline

Files:

- `src/java/billiards/viewer/Viewer.java`
- `src/java/billiards/viewer/Utils.java`

Issue:

Full redraws could submit per-pixel work and stale renders could overwrite newer renders.

Fix:

Changed redraw to row-batched futures. Added guarded render coalescing for the long-lived viewer executor. Coalescing is disabled when there is only one worker to avoid same-executor deadlock.

Philosophy:

Rendering should be parallel enough to be responsive, but old render jobs must not win the race and paint stale images after a newer user action.

Conditions:

Rapid zoom, reflect toggles, cover loads, large region render.

Significance:

Medium/high UI performance.

Validation:

Compile passed. Manual rapid UI redraw testing still required.

### OPT-006: Polygon Refinement Micro-Consolidation

Files:

- `src/backend/cpp/equations.cpp`
- `src/backend/cpp/refine.cpp`
- `src/backend/headers/refine.hpp`

Issue:

Most Nick Shan Linux P1/P2 parallel refinement was already present or ported differently, but one copy reduction remained useful.

Fix:

Moved refined polygon results instead of copying in `intersect_one_way`; clarified comments around parallel polygon intersection.

Philosophy:

Only port Linux changes that remain meaningful after Abdul Windows' existing TBB/worker-count implementation.

Significance:

Low performance, low risk.

Validation:

`testBackend` passed 31 native tests.

### OPT-007: Pattern Finder StringBuilder

Files:

- `src/java/patternfinder/PatUtils.java`
- `src/java/patternfinder/PatternFinder.java`

Issue:

Repeated `String +=` output construction could become O(n^2) for large pattern output.

Fix:

Converted the output-building paths to `StringBuilder`.

Philosophy:

Keep output formatting the same, but avoid copying the accumulated result on every append.

Significance:

Low/medium performance. Useful for large pattern output.

Validation:

Java compile and tests passed. Manual output comparison recommended.

### OPT-008: User Worker-Count Control

Files:

- `src/java/billiards/viewer/Main.java`
- `src/java/billiards/viewer/Utils.java`
- `src/java/billiards/wrapper/Wrapper.java`
- `src/backend/headers/utils.hpp`
- `src/backend/headers/wrapper.hpp`
- `src/backend/cpp/wrapper.cpp`

Issue:

Volunteers need a simple way to limit CPU use. Java and native code had separate knobs, and native default behavior did not cleanly follow the UI process.

Fix:

Added:

- `--threads=N`
- `--billiards-threads=N`
- Java default: half of `Runtime.availableProcessors()`
- User clamp: `1..availableProcessors-1`, or 1 on a 1-core machine
- Native default: half of `std::thread::hardware_concurrency()`
- Native setter: `backend_set_worker_threads(int32_t)`
- JNA bridge: `Wrapper.configureNativeThreads(Utils.numThreads)`

Philosophy:

Volunteer builds need one obvious launch argument. Java should own the release-facing setting and push the same resolved value into C++.

Conditions:

Any long calculation, cover, vary, AutoVary, or render-heavy testing.

Significance:

High for stability and volunteer machine usability.

Example:

```powershell
.\gradlew.bat --no-daemon run --args="--threads=2"
```

Validation:

Compile passed. Manual launch should confirm console output:

```text
Threads available: 2
```

### ARCH-001: Native DB Connection Ownership

File:

- `src/backend/cpp/wrapper.cpp`

Issue:

Some native picture-loading paths held SQLite pooled connections during expensive geometry compute.

Fix:

Split load into short existence check, connection-free compute, short save, and short final load.

Philosophy:

SQLite handles are scarce. Long native compute should not monopolize a DB connection while other workers are waiting.

Conditions:

Concurrent AutoVary/storage workers and normal picture loading.

Significance:

Medium/high concurrency stability.

Validation:

Compile passed. Concurrent DB stress test still recommended.

### ARCH-002: Database Path Relocation

Status:

Deferred by user.

Reason:

Nick Shan Linux moved database location, but Abdul Windows should not change user-data paths without migration planning.

### BUILD-001: Native C++ Test Runner

File:

- `build.gradle`

Issue:

The Windows backend linked with `-ltbb12`, but the native test executable linked with `-ltbb`. `testBackend` also did not put MSYS2 DLLs on `PATH`.

Fix:

Aligned Windows native tests to `-ltbb12` and added `${msys2NativePrefix}/bin` to `testBackend` PATH.

Philosophy:

Native tests must use the same dependency environment as `backend.dll`; otherwise tests fail even when production builds.

Significance:

Medium build/test reliability.

Validation:

`testBackend` ran 31 Boost tests with no errors.

### BUILD-002: Release Docs And Ignore Rules

Files:

- `.gitignore`
- `AGENTS.md`
- `docs/release/**`

Issue:

Useful docs existed outside the Abdul repo, and local generated artifacts were easy to commit accidentally.

Fix:

Added:

- `docs/release/README.md`
- `docs/release/BUILD-RUN-GUIDE.md`
- this report
- copied human-readable reference docs into `docs/release/reference`
- repo-local `AGENTS.md`
- expanded `.gitignore` for Gradle caches, IDE metadata, temp files, cover output, crash/profiling dumps

Philosophy:

The release bundle should not depend on a separate workspace docs folder. Future agents should also have repo-local rules.

Important caveat:

Some generated files such as `.gradle`, `cover`, `tmp`, and `middleVary3.txt` were already tracked historically. `.gitignore` does not untrack existing files. Removing those from version control should be a deliberate separate cleanup.

## Gradle And Build Changes

The build now supports the practical Windows path:

- Detects MSYS2 root from `MSYS2_ROOT`, defaulting to `C:/msys64`.
- Picks `BILLIARDS_MSYS2_PREFIX` if set.
- Otherwise prefers UCRT64 when `ucrt64/bin/g++.exe` exists.
- Adds MSYS2 include and Eigen include paths on Windows.
- Links Windows backend with `-lgmp -lmpfr -lmpfi -lsqlite3 -ltbb12 -lboost_thread-mt -lws2_32`.
- Links Windows native tests with `-lboost_unit_test_framework-mt ... -ltbb12 -lboost_thread-mt -lws2_32`.
- Adds MSYS2 bin to runtime `PATH` for `run`.
- Adds MSYS2 bin to `testBackend`.
- Keeps Java target/source at 17.
- Keeps user runtime heap settings at 2g/6g/2g.
- Adds native debug option with `-PbilliardsNativeDebug=true`.
- Keeps `-Og` instead of `-O0` for native debug because unused legacy helper references can make `-O0` link fail.
- Keeps compile load lower with `gradle.properties`.

## New User-Facing Behavior

### Main Calculate Add To Cover

The top input row now has:

```text
[code input] [Calculate] [Add to Cover]
```

This is intentionally a checkbox, not a new action button. It modifies the existing Calculate behavior only when selected.

Duplicate behavior:

- Exact matching stable line already in Cover -> skip.
- Exact matching triple line already in Cover -> skip.
- Different comments/spacing can still be treated as different lines.

### Thread Count

Default:

```text
half of available logical processors
```

Override:

```powershell
.\gradlew.bat --no-daemon run --args="--threads=4"
```

Bounds:

- minimum 1
- maximum `availableProcessors - 1`
- if the machine reports 1 processor, maximum is 1

Native C++:

Java pushes the resolved worker count into C++ with `backend_set_worker_threads`. The backend keeps a `tbb::global_control` alive so direct TBB work follows the same cap as Boost worker pools. Native-only scripts can still use `BILLIARDS_NATIVE_THREADS` when Java does not call the setter.

Some native algorithms still pass an explicit local maximum to `billiards_worker_count(max)`. Those caps remain because they are algorithm-specific safety limits.

### 2026-07-15 Concurrency And MRR Addendum

The detailed source-to-failure analysis is in `CONCURRENCY-MRR-FIXES-2026-07-15.md`.

The long MRR bug was a representation-invariant failure, not ordinary unsynchronized memory access. Independent workers refined copies of the original bounding polygon and the main thread intersected those partial polygons. The geometric intersection could be correct while retaining a linear bounding edge. MRR serialization requires every final edge to carry the trigonometric equation and left/right witness that created it, so the surviving `-x-y+2eta` edge reached `RearrangeVariant` and triggered the reported exception.

The dependent curve-reduction order is now deterministic and complete. MRR remains parallel where results are independent: unfolding/curve generation uses worker-local containers, and each curve refinement classifies polygon corners with TBB. The configured `--threads` value caps TBB, and a fair interruptible Java lock prevents several outer MRR calls from multiplying that worker budget.

Native return values now retain their mathematical meaning: `0` is a certified empty region, `1` is nonempty, and `-1` throws with the native diagnostic. `Database` therefore caches certified empty/nonempty results but never caches an internal failure as empty.

AutoPolyVary now advances already-covered coordinates in one loop. A per-run controller owns the current task, executors, progress, and an atomic terminal state. This removes the old recurse-without-return branch that created multiple task chains, repeated coordinates, premature progress completion, duplicate cover lines, and repeated cancel banners.

Validation on 2026-07-15:

- `compileJava backendSharedLibrary` passed.
- `testBackend test` passed; the native runner executed 33 cases.
- `testBackendSlow` passed the exact reported 124-number CS with one and four workers and identical boundary data.
- Java forward/reverse AutoPolyVary index tests passed.

### BUG-007 Main-Window Shutdown Follow-Up

The existing shutdown fix exposed an older nullable lifecycle assumption in `IterateToLimitWindow`. AutoVary can construct that window only as a LiPattern result sink, without calling `execute()`. In that state its `finish` observer is null. Main-window shutdown calls `IterateToLimitWindow.close()`, whose unconditional `finish.set(true)` produced the reported `NullPointerException` before shutdown completed.

Manual close, task completion, and programmatic close now use one null-safe finish notifier. A real observer is still set to true when present; a result-sink-only window still clears transient results, saves all text, and closes without trying to notify a nonexistent observer. Java tests cover both null and initialized observer states.

### BUG-012 AutoPolyVary Counter/Redraw Race

AutoPolyVary previously changed the viewer map for the next OBO coordinate and immediately searched the current region image for uncovered points. The map change is synchronous, but the coalesced image redraw is asynchronous. Candidate discovery could therefore read the previous coordinate's image through the next coordinate's map, produce an empty point list, and complete a zero-work task. Each zero-work completion advanced again soon enough to cancel the pending redraw, which explains the counter racing to 4444 after only the first coordinate performed vary work.

Coordinate work is now gated by the matching image-commit callback. Rendering still runs in the background, the JavaFX thread commits the completed frame, and only then does AutoPolyVary inspect pixels and start vary. The terminal reports every point and its uncovered candidate count, including an explicit message for a legitimately fully covered view. The Java ordering regression test and `backendSharedLibrary test` passed on 2026-07-15.

### 2026-07-16 Developer Handbook, Debugger, And Benchmark Tooling

`docs/CODEBASE-HANDBOOK.md` is now the authoritative current-source learning path. It starts from the research workflow and progressively traces JavaFX handlers, Java domain objects, JNA/native ownership, C++ algorithms, mathematical representations, concurrency, testing, mixed debugging, and benchmarking. Older project/source/bug studies remain archived evidence rather than parallel manuals.

`tools/docs/audit-handbook.ps1` invokes Universal Ctags in structured JSON mode, parses symbols rather than source text, generates native Doxygen XML, and checks stable handbook markers. Its generated Section 14.3 accounts for production and test fields, methods, classes, functions, types, and cases; the prose ledger separately distinguishes a listed symbol from a sufficiently explained algorithm. Because this Windows Ctags build otherwise calls unavailable Unix `sort -u`, the audit explicitly uses `--sort=no`. Windows PowerShell execution-policy commands use process-scoped `-ExecutionPolicy Bypass` and do not alter machine policy.

`.vscode/tasks.json` and `.vscode/launch.json` add JDK 17 Java/JDWP attachment, UCRT64 GDB attachment to the same Java process, direct native Boost.Test debugging, an exact long-MRR debug launch, build/test tasks, documentation audit, and benchmark launch. The expected local tool paths are documented in the handbook.

`tools/benchmark/` adds named small, medium, and large correctness-first workloads. The large case is the exact reported CS from BUG-009. A gated Boost case runs it once at a selected worker count and prints a stable FNV-1a regression digest built from interval endpoints, boundary equations, and `LeftRight` provenance. The runner builds outside timed samples, records Git/host/JVM/profile metadata, captures per-process wall time, CPU time when available, peak working set, stdout/stderr, warmups, and raw samples, and fails when the correctness signal is absent or hashes differ.

Validation on 2026-07-16:

- `compileJava backendSharedLibrary test testBackend` passed; the native suite ran 34 cases.
- The final handbook audit reached 100 percent marker coverage for 2,708 current production/test symbols and generated native Doxygen XML.
- A one-sample small workload smoke matrix completed for worker counts 1 and 2 and wrote a valid evidence bundle under `build/benchmarks/`.
- JDK 17 was selected through `JAVA_HOME`; JDWP listening/attachment and UCRT64 GDB symbol resolution for Java-hosted native code and the Boost test executable were checked.
- The exact long-CS workload completed one measured sample at 1, 2, and 4 workers with the same full-boundary hash `883e8b0c1317184e`; observed wall times were 8.069 s, 5.538 s, and 4.356 s. These are smoke timings, not a five-sample performance conclusion.

The benchmark tools do not claim a performance improvement yet. A defensible result needs at least five optimized measured samples per worker count under matched metadata, plus identical correctness hashes.

### 2026-07-31 `nextIter` MRR and Cancellation Repair

Author: Abdul

Date: 31 July 2026

The three reported candidates are expected to calculate. Their failures were
native compatibility regressions from stricter equal-sign and connected-arc
gates, not candidates that AutoPolyVary should skip. The skip-only Java types,
counters, and success summaries were removed. Production intersection and
refinement now follow the established full/half-segment behavior and still
require exact zero validation in both equations. The exact three code
sequences pass complete slow MRR regressions.

Cancel is now latched for the complete native-Vary operation. Workers cannot
clear the flag, queued calls cannot enter native code after cancellation, all
native-Vary workflows share one exclusive key, and both Cancel buttons and
window close controls signal native cancellation.

The Windows packaged launcher now receives the same documented JVM memory
settings as Gradle, and startup exposes the actual JVM arguments and maximum
heap. PatternFinder now shares the configured `--threads` budget. Structured
native logging, pre-return native Vary streaming, and LiPattern CPU-parking
profiling remain explicitly tracked follow-ups. Full details and the exact
reported native regression are in
`docs/release/NEXTITER-STABILITY-2026-07-31.md`.

## Known Remaining Risks

### Manual UI Testing Still Needed

Compile tests cannot prove JavaFX layout correctness. Volunteers should test:

1. Main viewer opens with the image centered and not shifted down.
2. Reflect off/on does not move the image vertically.
3. Calculate with `Add to Cover` adds one stable line.
4. Repeating the same Calculate does not duplicate the line.
5. Valid stable-unstable-stable triples add once to the triple cover area.
6. Unstable singles do not get incorrectly written as stable cover entries.
7. Cover window text persists when closed and reopened.
8. Cover, Small Cover, and Stables save state when the main app closes.
9. AutoVary and CycleVary cancel preserve partial progress.
10. Rapid zoom/toggle/load actions do not flash stale renders.
11. AutoPolyVary traverses the reported 170-173 region once, reaches the true end once, and prints only one cancel/success banner.
12. AutoPolyVary cover and small-cover output contain no duplicate exact code lines.
13. Let AutoVary create the LiPattern result sink without opening LiPattern, then close the main window and confirm there is no `finish` null dereference.
14. Run AutoPolyVary through the reported line-4444 range and confirm the counter waits for each redraw/calculation; each point should print its uncovered candidate count instead of advancing silently.

### Memory Testing Still Needed

The old Main runtime reportedly reached very high virtual memory after large OSNO work and did not release it promptly. Abdul Windows now has evaluator reuse and several allocation-pressure fixes, but native memory should still be tested with `docs/procwatch.ps1`, Task Manager, or VMMap:

1. Launch with `--threads=2`.
2. Run a small cover.
3. Run a known large OSNO cover.
4. Cancel during a large run.
5. Repeat the run.
6. Confirm memory plateaus after worker warmup instead of increasing unboundedly.

`docs/procwatch.ps1` is the quick first-pass tool for this: start the app, run the script from another PowerShell, and watch private memory, virtual memory, working set, CPU time, and thread count once per second.

### Database Race Testing Still Needed

The wrapper now releases DB connections during compute. This is better for pool pressure, but concurrent workers can still race to compute and save the same missing code. Test concurrent AutoVary/storage flows and watch for duplicate-save SQLite errors.

### Database Relocation Deferred

Do not move database paths for this release.

## Release Smoke Test Checklist

Build:

```powershell
.\gradlew.bat --no-daemon compileJava backendSharedLibrary testBackend test
.\gradlew.bat --no-daemon testBackendSlow
```

Launch:

```powershell
.\gradlew.bat --no-daemon run --args="--threads=2"
```

UI:

- Main viewer not shifted.
- Reflect toggle stable.
- Calculate stable with add-to-cover.
- Duplicate stable skipped.
- Calculate triple with add-to-cover.
- Duplicate triple skipped.
- Cover window close/reopen preserves text.
- Main window close saves Cover/Small Cover/Stables text.
- Start VaryL, PolyVary, CycleVary, Cover, and lookup work, then close/cancel;
  confirm no late success/error dialog or render is published.
- Reopen/edit SuperPolyVary while a run is active; confirm the first run keeps
  its original schedule and options.
- Pan/zoom during a delayed render; confirm hole finding waits for or uses the
  matching committed camera/raster pair.
- Inject or simulate a failed cover merge; confirm the prior live directory is
  unchanged and a successful merge retains one rollback generation plus
  `manifest.sha256`.
- Confirm the update button remains disabled and no updater script is shipped.
- Do not exercise Small Cover as a release-ready workflow; `BUG-166` and
  `BUG-168` remain explicitly dormant.

Performance:

- Run one small cover.
- Run one larger OSNO cover.
- Watch RAM and virtual memory.
- Run AutoVary near a zoomed/reflected edge.
- Cancel a long AutoVary and confirm partial progress remains.
- Use the benchmark runner at 1/2/4 workers and require repaired hash
  `9f837d3bd0c612a7`; compare certificate timings under `OPT-027` without
  weakening proof tests.

## Reference Docs Included

Copied into this repo:

- `docs/release/reference/current-tracker/`
- `docs/release/reference/project-study/`
- `docs/release/reference/bug-audit/`
- `docs/release/reference/source-study/`

Raw Ghidra/CFR/class extraction artifacts were not copied into this release folder because they are large and not needed for volunteer setup. They remain in the wider workspace docs tree that was used as evidence during analysis.
