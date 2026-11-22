# Westeros GL BRCM L1 tests

These tests compile the production `westeros-gl.cpp` translation unit with `UNIT_TEST` and link deterministic Nexus/NXPL/BKNI platform doubles. The tests exercise public display queries and safe-area math, display-size listener registration/change/removal, native-window delegation, pixmap allocation/reuse/recreation, copy/checkpoint flow, cleanup, and initialization failure handling without requiring Broadcom hardware.

Run from the repository root on Linux:

1. Install CMake, a C++ compiler, GoogleTest, and gcovr (gcovr is needed only for coverage).
2. Configure: `cmake -S L1 -B L1/build -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON`.
3. Build: `cmake --build L1/build --parallel`.
4. Test: `ctest --test-dir L1/build --output-on-failure`.
5. Coverage: `cmake --build L1/build --target coverage`.

Coverage is scoped only to the production implementation `westeros-gl/westeros-gl.cpp`; the coverage target enforces a 75% line threshold. This is a defined measurement goal, not a reported result until the target is run.
