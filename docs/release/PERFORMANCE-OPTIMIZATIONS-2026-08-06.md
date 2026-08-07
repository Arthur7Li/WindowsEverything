# Performance and Stability Optimizations (2026-08-06)

This release implements three critical performance and stability optimizations targeted at improving the reliability of `LiPattern` and the efficiency of `LiMV` and broad cover calculations.

## Changes Implemented

1. **LiPattern Exception Propagation (BUG-020)**
   - Modified `SuperCheckTask.java` to explicitly propagate verification exceptions back to the JavaFX lifecycle.
   - Prevents backend crashes or SQL parser errors from being silently treated as valid pattern removals, thereby avoiding data corruption in the user's sequences.

2. **Cartesian Product Memory Bounding (BUG-030)**
   - Modified `bounding_inequalities.cpp` inside `eliminate_phi` to replace a vector-based buffer with a thread-local `std::set`.
   - Eliminates the previous $O(N \log N)$ sort-thrashing behavior when buffers reached capacity.
   - Introduced a hard 1,000,000 item memory ceiling that gracefully throws `std::runtime_error` rather than causing an unrecoverable system freeze.

3. **Early Rational Bounding Rejection (OPT-010)**
   - Modified `equations.cpp` so that `calculate_stable` and `calculate_unstable` compute exact rational bounds before generating costly trigonometric Unfoldings.
   - Mathematically impossible sequences are rejected instantly, drastically improving batch throughput for impossible regions during `LiMV` and Cover runs.

## Verification
Both `testBackend` and `test` suites pass cleanly, guaranteeing geometric correctness constraints are preserved alongside these optimizations.
