# Handbook Rewrite Work State

Last updated: 2026-07-31

## Authority

- Active source: Abdul Windows fork.
- Reader-facing book: `docs/CODEBASE-HANDBOOK.md`.
- Reproducible reader editions: `tools/docs/build-handbook.ps1` generates the
  ignored `build/handbook/CODEBASE-HANDBOOK.html` and `.pdf` artifacts.
- Source baseline: branch `main`, commit
  `0d2be2093bed5b4673b855123f1ff8ce16d2fa20`, plus the uncommitted
  evidence-first and critical-repair campaigns documented in
  `docs/release/EVIDENCE-FIRST-REPAIRS-2026-07-27.md` and
  `docs/release/CRITICAL-REPAIR-CHECKPOINT-2026-07-27.md`.
- Coverage ledger: `docs/handbook/coverage.csv`.
- Generated inventories: `build/doc-audit/`; they are evidence, not prose.

## Completion Contract

A symbol name or generated row is zero explanatory credit. A behavior-bearing
symbol is complete only after its handbook entry explains purpose, contract,
source-order behavior, meaningful branches, invariants, callers/callees, a
worked example, failure modes, and optimization constraints. Trivial symbols
must be grouped with a written rationale.

## Current Gate

- Gate: 11, critical-repair integration and complete source re-audit.
- Status: completed for the current 4,310-symbol production inventory.
- Current authoritative checkpoint: handbook Sections 14.182 through 14.190
  explain the repaired exact-gradient and intersection certificates,
  operation lifetime and terminal publication, immutable search requests,
  typed reconstructed-storage validation, Cover triple/artifact transactions,
  committed camera/raster rendering, Save V3 staging, disabled updater, and
  performance constraints, `nextIter` MRR compatibility, native-Vary cancellation, observable JVM
  settings, and the unified PatternFinder worker budget. The Doxygen-enabled 2026-07-31 audit is
  4,310 / 4,310 production symbols (100%): 2,951 reviewed entries plus 1,359
  justified trivial groupings, with 143 support-test rows excluded and zero
  explanatory gaps.
- Completed layers: Gates 1-11 now cover mathematical vocabulary, code-number
  effects, Pattern/Expando, symbolic equations, unfolding, bounding and
  refinement, proof/cover/triples/Vary/database/JNA/concurrency, Java/UI
  pipeline dossiers, every remaining source-order symbol, the final
  explanatory/quality audit, and the July critical repair.
- Historical checkpoints from Sections 14.28-14.181 remain useful provenance,
  but none is the resume boundary. The authoritative handbook continuation
  state is the zero-gap Section 14.189 checkpoint above. The wider code-repair
  continuation state remains the critical-repair checkpoint file.
- Next exact unresolved source boundary: none in the current explanatory
  inventory. A resumed handbook task should start with reader-driven editorial
  revision or a source change that makes ledger hashes stale, not another blind
  symbol pass. The later instructions PDF remains a separate agenda item.
- Preserve the distinction between mathematical empty, disproved, verified,
  unsupported, cancelled, and backend failure across C++/JNA/Java.
- Production and test changes in the repair campaign are user-authorized and
  source-evidenced. Keep the live bug register and release reports synchronized
  before declaring the campaign complete.

## Gate Order

1. C++ vocabulary and mathematical representations. **Completed.**
2. Code sequences and the exact effect of changing code numbers. **Completed.**
3. Pattern and Expando. **Completed.**
4. Symbolic algebra and trigonometric equations. **Completed.**
5. Unfolding, shooting vectors, and equation construction. **Completed.**
6. Bounding inequalities and regions. **Completed.**
7. Interval evaluation, Newton/intersection, and exhaustive refinement cases.
   **Completed.**
8. Verification, cover, triples, Vary/search, database, JNA, and concurrency.
   **Completed.**
9. Minimal Java/UI semantic pointers and end-to-end pipeline dossiers.
   **Completed.**
10. Full explanatory audit and sampled human review. **Completed.**
11. July critical-repair integration and complete affected-file re-review.
    **Completed.**

## Final Quality Gate

- Inventory audit with Doxygen: 4,310 / 4,310, zero gaps.
- Ledger arithmetic: 2,951 `reviewed` + 1,359 `grouped-trivial` = 4,310;
  143 nonproduction support-test rows are excluded.
- Inventory and ledger identity: 4,453 rows and 4,453 unique IDs. `DOC-004`
  assigns source-order ordinal suffixes to the 55 Ctags rows in 20 apparent-ID
  collision groups and makes either inventory or ledger duplication fatal.
- Sampled source-to-book review covered symbolic math, refinement, cover
  recursion, native ABI ownership, Java Storage, cancellation/progress,
  Viewer workflows, dormant prototypes, exact geometry, Cover artifacts,
  database administration, and every source file changed by the critical
  repair.
- The sample found and corrected the exact slash/space behavior in
  `CoverStuff` prose. It also normalized 120 corrupted/curly quote sequences;
  the handbook now contains zero non-ASCII and zero mojibake characters.
- Doxygen completed with 25 nonfatal template/member-matching warnings (eight
  each in `unfolding.cpp`/`unfolding.hpp`, four each in
  `evaluator.cpp`/`evaluator.hpp`, and one typedef-signature warning in
  `triangle_billiard4.cpp`). They are retained in
  `build/doc-audit/doxygen-warnings.log`; the source/ctags ledger remains the
  authority for those template specializations.
- The live `bug-register.csv` remains the authority for resolution status.
  Re-run its uniqueness/reference checks after the critical rows and
  performance follow-up are finalized; do not reuse an existing ID.
- A direct handbook whitespace scan finds only four intentional trailing-space
  lines: two Markdown hard breaks in the header and two literal trailing-space
  examples in the native Vary transport grammar. Existing unrelated dirty
  worktree files were not reverted or modified by this documentation
  continuation.

## Reader Editions

- The Markdown file remains authoritative. `handbook-render.lua` removes only
  its duplicate source title for publication, promotes the remaining heading
  hierarchy without changing anchors, and fails the build if TeX appears
  outside supported math delimiters where HTML would silently omit it.
- `handbook-print.css` provides a standalone browser layout and letter-sized
  print layout with a title page, two-column contents, portable Courier New
  code text, wrapped code blocks, tables, MathML, internal destinations, and
  print-safe page-break rules.
- The final 2026-07-31 build used the installed Pandoc and Headless Chrome. It
  produced a 2,407,376-byte self-contained HTML file and an 11,039,763-byte
  PDF with a valid `%PDF-` signature. The HTML contains 421 MathML nodes.
- Validation: the full builder exited zero after rerunning outside the
  filesystem sandbox so Headless Chrome could write the PDF;
  `audit-handbook.ps1 -Mode enforce` with Doxygen reached 4,310 / 4,310.
  Doxygen retained the same 25 nonfatal template/member-matching warnings.
- Rebuild after every reader-facing handbook edit with
  `.\tools\docs\build-handbook.ps1`. Generated HTML, PDF, extracted text, and
  raster-validation files stay below ignored `build/` and are not source.

## Resume Procedure

1. Read `AGENTS.md`, this file,
   `docs/release/CRITICAL-REPAIR-CHECKPOINT-2026-07-27.md`, and
   `build/doc-audit/coverage.md`.
2. Confirm `git rev-parse HEAD`. The documented repair baseline is
   `0d2be2093bed5b4673b855123f1ff8ce16d2fa20`; no commit or push is authorized
   by the campaign prompt.
3. If production/test source changes, run the audit with `-SyncLedger`, author
   the affected behavior, and review only the invalidated rows. Never restore
   status solely to make the percentage pass.
4. Preserve the user's five pre-campaign dirty paths listed in the critical
   checkpoint and keep `BUG-166`/`BUG-168` dormant unless the user changes
   scope.
5. Re-run Doxygen enforcement and the reader builder after handbook edits.
   Record exact validation and continuation state before ending.

## Evidence And Decisions

- The old audit reported 100 percent because it counted generated Ctags markers.
  That metric is retired.
- The corrected audit includes private/file-scoped helpers and currently
  inventories 4,063 production symbols.
- The raw generated appendix occupied most of the book and is being removed.
- The user wants mathematical/backend depth and only brief UI wiring coverage.
- Source-derived research effects are explained; expert intent that source
  cannot establish is labeled `[UNRESOLVED]` rather than invented.
- Documentation-only work is in scope. New tests and production trace hooks are
  deferred.
- The 2026-07-17 continuation added Sections 14.28-14.32 as completed work;
  Sections 14.33-14.40 are provisional reconnaissance only. BUG-052 through
  BUG-072 plus OPT-011 through OPT-016 remain source-evidenced agenda items.
  The high-priority defects include
  triple stable-polygon omission, concurrent cover artifact publication,
  MRR stable proof geometry reconstructed from double midpoints, duplicate
  diagnostic aggregation returning only the last triple, and SQLite's unbounded
  `SQLITE_BUSY` hot-spin retry. Do not relabel these findings when their source
  boundaries recur.
- No production or test source was changed in this handbook pass. The native
  wrapper rewrite added BUG-073 (partial allocation leak in
  `calculate_gradient`) to the agenda; the Java/JNA pass added BUG-074
  (ALL-info error erasure), BUG-075 (gradient return ABI mismatch), and
  BUG-076 (dropped invalid Vary rows). It did not silently fix any of them.

## Next-Section Reconnaissance Already Completed

- `wrapper.cpp` is the native ABI boundary immediately after the documented
  data layer. Its first function is `to_cstr` at line 49; later functions catch
  exceptions, preserve `backend_last_error`, construct WAL/full-sync pools, and
  expose MRR, cover, search, and information operations to Java.
- `sqlite_error_logging` relies on a one-time-before-open SQLite requirement.
  The wrapper's boot order and Java lifecycle must be documented before treating
  repeated application startup or pool recreation as safe.
- Preserve the distinction between a Java-visible native error, an ordinary
  mathematical non-result, a user cancellation, a SQLite busy wait, and a
  successful proof whose shared artifact publication failed.

## Unresolved

- Expert reasons for choosing one research parameter over another cannot always
  be recovered from code. Formal parameter effects can and must still be shown.
- Each source TODO or apparently unreachable refinement case must be explained
  as implemented before deciding whether it is a bug.
