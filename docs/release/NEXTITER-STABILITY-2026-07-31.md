# `nextIter` MRR and Cancellation Repair

Author: Abdul

Date: 31 July 2026

## Reported failures

`docs/nextIter.txt` records three long AutoPolyVary candidates that reached
native MRR refinement and failed with either an equal-endpoint
`inconclusive polygon refinement` diagnostic or an inability to certify one
unique common root on a connected boundary arc. The same report says Cancel
stopped ending the active calculation.

These candidates are expected research inputs, not optional work. The earlier
candidate-skipping change was therefore incorrect: it hid a native
compatibility regression, discarded an expected MRR, and allowed the larger
search to appear successful. That Java policy and its typed skip-only classes
have been removed.

## MRR compatibility repair

The production refinement path again follows the established Abdul/Main
behavior:

- `refine_line_segment` and `refine_polygon` no longer reject an edge merely
  because its exact endpoint signs are equal and nonzero;
- `intersection` tries the complete segment and both half-segments, as the
  established implementation did;
- `intersection_zero` again uses the established midpoint, central-quarter,
  and inward-fudge seeds when one endpoint is already a known root;
- every accepted Newton result is still checked by substituting it into both
  equations and requiring exact zero signs; and
- failure to obtain a validated common root still raises an ordinary native
  error. It is not converted to an empty result and is not skipped by Java.

The newer Krawczyk, connected-arc, and concealed-crossing helpers remain in the
source and in focused proof tests. They are useful research machinery, but
they no longer sit in the production path as stricter acceptance gates that
reject previously supported inputs. Any future attempt to promote those gates
must first prove compatibility against the complete established MRR corpus.

PolyVary, VaryL, and CycleVary now call `Database.loadStorage` directly again.
A present `Storage` is published, a certified empty result remains
`//empty set`, and any genuine native/database/programming failure fails the
task. The deleted `NativeMrrException` and `VaryStorageLoader` classes, their
skip counters, terminal summaries, and their tests are not part of the
program.

## Cancellation repair

The native cancel flag is now operation-scoped instead of worker-scoped.
Individual VaryCS, Vary3, and Vary4 workers never clear the process-wide flag.
The Java owner resets it once, only after acquiring the shared native-Vary
operation key. Cancel uses a release store and worker polling uses acquire
loads.

Java also keeps an atomic cancellation latch. Pressing Cancel or closing any
of the Vary progress windows sets that latch before signalling native code.
A native call already running observes the native flag. A call waiting for the
Java native-admission lock checks the latch after it wakes and terminates as a
cancellation instead of entering native code after the first worker returns.
All Boyan, Tetra/Bar, PolyVary, VaryL, MiddleVaryL, AutoPolyVary,
SuperPolyVary, and CycleVary entry points use the same exclusive operation key,
so a later workflow cannot reset cancellation while an earlier one is still
retiring.

## JVM and worker configuration

The Windows packaged launcher receives the same `-Xms2g`, `-Xmx6g`,
`-XX:MaxDirectMemorySize=2g`, stack, server, and diagnostic defaults used by
Gradle. Startup prints the effective JVM arguments, maximum heap, and worker
count. PatternFinder consumes `billiards.viewer.Utils.numThreads`, so its Java
pool uses the same requested limit as the Viewer and native backend.

These settings bound configured pools; they do not promise that every worker
will remain runnable during sequential certification, merge, or database
phases.

## Incremental output boundary

Successful `Storage` objects continue to enter observable partial-result lists
as Java processes completed candidate futures. Native Vary still returns one
complete serialized result per native call, so discoveries made inside an
interrupted native call cannot yet be streamed or retained. The bounded
callback/chunk ABI needed for that behavior remains `OPT-029`; this repair does
not pretend that skipping a failed MRR is incremental output.

## Validation

Validation completed on 31 July 2026:

- `compileJava --rerun-tasks` passed.
- `testBackend` passed all 50 native tests.
- `testBackendSlow` passed all 50 native tests in 1 minute 29 seconds after the
  final ordinary and zero-endpoint compatibility restoration. Its
  slow gate calculates the complete MRR for each of the three exact reported
  `nextIter` code sequences with one worker and requires a polygon with at
  least three points.
- The complete Java test suite passed against a freshly linked side-by-side
  `backend.dll`, including JNA ABI and cancellation-latch coverage.
- The ordinary Gradle link destination was temporarily locked by a running
  Java application. The same compiled objects linked successfully to
  `build/verification-native/backend.dll`; no running user process was killed
  and no locked binary was overwritten.

Manual acceptance remains useful: replay the reported AutoPolyVary input,
confirm those candidates produce MRRs rather than skip warnings, press Cancel
while several native calls are queued, and repeat using the window close
control. Both cancel paths should stop without a later queued call restarting
native work, while already published results remain visible.

## Deferred items

- `OPT-028`: structured backend logging.
- `OPT-029`: incremental native Vary result transport.
- `OPT-030`: measured LiPattern/OSNO parked-CPU and phase-utilization study.
- `OPT-027`: proof research and optimization outside the compatibility path.
