# Abdul Windows Release Docs

Start here for the July 2026 Abdul Windows handoff. The 28 July critical
repair campaign is the newest release layer; earlier reports remain historical
evidence for the changes they describe.

## Release-Facing Docs

- `ABDUL-WINDOWS-RELEASE-REPORT.md` - main human-readable report of bugs, fixes, optimizations, Gradle/native build changes, risks, and test checklist.
- `BUILD-RUN-GUIDE.md` - practical Windows, macOS, and Linux setup and launch guide for volunteers.
- `CONCURRENCY-MRR-FIXES-2026-07-15.md` - source-to-failure explanation and validation record for the MRR and AutoPolyVary concurrency fixes.
- `CALCULATE-ADD-TO-COVER-FIX-2026-07-20.md` - database-free reproduction and result-reuse fix for the reported long OSNO Calculate/Add-to-Cover discrepancy.
- `EVIDENCE-FIRST-REPAIRS-2026-07-27.md` - persistence evidence, fixes, tests, limitations, and manual checks for the gradient ABI, Vary4 beam geometry, restricted MRR merge, Save V3 transaction, and transform-capacity pass.
- `CRITICAL-REPAIR-CHECKPOINT-2026-07-27.md` - exact implementation and validation chronology through completed checkpoint 11, including native proof certificates, application-owned operations, immutable requests, cover transactions, camera-bound renders, updater removal, digest audit, and the retained 1/2/4-worker benchmark.
- `NEXTITER-STABILITY-2026-07-31.md` - Abdul's reported AutoPolyVary/MRR failure, candidate-level continuation policy, packaged JVM settings, shared worker budget, partial-result behavior, tests, and explicitly deferred logging/streaming/performance work.
- `SIGNIFICANT-APPLICATION-CHANGES.md` - neutral technical report on the significant correctness, concurrency, memory, algorithm, Linux-derived, and research-workflow changes, with detailed code excerpts and implementation constraints.
- `../CODEBASE-HANDBOOK.md` - authoritative source-level behavior and maintenance constraints. Sections 14.182-14.190 cover the critical repair, `nextIter` MRR compatibility, and native-Vary cancellation; the Doxygen-enabled ledger currently reports 4,310/4,310 production symbols.

## Reference Docs

The `reference/` folder contains copied study material from the wider workspace so this repo can be handed off without relying on external paths:

- `reference/current-tracker/` - current Abdul tracker files from `docs/codex-project-study/`.
- `reference/project-study/` - architecture, comparison, build, math, and plan docs from the earlier project study.
- `reference/bug-audit/` - bug-audit summaries and tracker files.
- `reference/source-study/` - source-study summaries and symbol/function indexes.

Large raw decompiler/Ghidra artifact trees were not copied into this release folder because they are noisy and not useful for volunteer setup. The report names them as evidence sources when relevant.
