# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

A personal fork of upstream [EPIJudge](https://github.com/adnanaziz/EPIJudge) (Elements of Programming
Interviews). It is a practice workspace, not a product: 220 problem stubs per language, each a
standalone `main()`, plus a shared test harness and a directory of TSV fixtures.

Work happens on `akhil/first_pass` (branched off `master`, which tracks upstream). 24 of 220 C++
problems and 9 of 226 Python problems are currently solved.

The C++ side has been substantially reworked in this fork — Conan 2 + CMake presets + multiple
toolchains. **`epi_judge_cpp/README.md` is the authoritative C++ document**; the repo-root
`README.md`, `CHANGELOG.md` and `.github/workflows/` are upstream's and describe a `g++ -std=c++14`
Makefile flow that no longer applies here.

## Repository shape

```
epi_judge_{cpp,python,java}/            stubs you solve
epi_judge_{cpp,python,java}_solutions/  upstream's reference answers
  └── test_framework/                   the judge harness, mirrored per language
test_data/*.tsv                         fixtures, one per problem, shared by all languages
config.json                             harness timeout + stop-on-failure, read at startup
solution-walkthrough.md                 personal per-problem reasoning notes
index.html, problem_mapping.js          generated progress tracker (see below)
```

Problem name is the spine: `reverse_bits.cc` ↔ `reverse_bits.py` ↔ `test_data/reverse_bits.tsv` ↔
CMake target `reverse_bits`.

## The harness contract

Every problem file ends with a `main()` that hands the harness a function pointer:

```cpp
int main(int argc, char *argv[]) {
  std::vector<std::string> args{argv + 1, argv + argc};
  std::vector<std::string> param_names{"x", "i", "j"};
  return GenericTestMain(args, "swap_bits.cc", "swap_bits.tsv", &SwapBits,
                         DefaultComparator{}, param_names);
}
```

To test a different implementation of the same problem, swap the function-pointer argument — that is
the intended way to compare attempts, and several files keep alternatives side by side.

**Do not edit `test_framework/`.** It is upstream code; changes there produce merge conflicts on
every update. Study it freely, but fix problems in your own file instead.

## C++ — build and run

Everything runs from `epi_judge_cpp/`. `build.sh` is the driver; it wraps Conan + CMake presets and
handles both runtime traps below.

```sh
./build.sh                             # setup + configure + build, gcc-debug
./build.sh build gcc-debug parity      # one target
./build.sh run   gcc-debug parity      # run with correct cwd and flags
./build.sh solved gcc-debug            # run every solved problem, report pass/fail
./build.sh all                         # all four toolchains, sequentially
./build.sh compdb clang-debug          # point clangd at a toolchain
./build.sh clean
```

Toolchains are `gcc-debug`, `gcc-release`, `clang-debug`, `clang-release`, plus `gcc-asan-debug`
(ASan+UBSan) and `gcc-tsan-debug`. gcc uses libstdc++, clang uses **libc++** on purpose — it catches
non-portable libstdc++ assumptions gcc never flags. ConanCenter has no binaries for gcc 16 / clang 22,
so the first `setup` per profile builds every dependency from source and is slow.

### Two traps when running a judge binary by hand

1. **Working directory matters more than `--test-data-dir`.** `GetDefaultTestDataDirPath()`
   (`test_framework/test_utils.h:90`) probes `test_data`, `../test_data`, … four candidates deep, and
   does so *before* `--test-data-dir` is parsed and outside any try/catch. From the wrong directory
   the binary aborts with `Can't find test data directory` regardless of flags. This is why build
   trees are exactly `build/<toolchain>/` and no deeper.
2. **Every run rewrites `problem_mapping.js`** — `update_js` defaults true
   (`test_framework/test_config.h:55`), dirtying the git tree. Pass `--no-update-js`.

Manual equivalent of `./build.sh run`:

```sh
cd <judge root> && ./epi_judge_cpp/build/gcc-debug/lru_cache \
    --test-data-dir test_data --no-update-js
```

Useful additions: `--no-tty --no-color` for clean logs, `-h` for the rest.

### Test granularity

One problem binary is the smallest runnable unit — the harness has **no flag to select an individual
test case**. The complete CLI is `--test-data-dir`, `--force-tty`, `--no-tty`, `--force-color`,
`--no-color`, `--no-update-js`, `--no-complexity`, `--help` (`test_framework/test_config.h:79-95`).
To narrow further, edit the TSV or add a temporary early-return.

`config.json` shapes every run: `numFailedTestsBeforeStop: 1` stops at the first failure (raise it to
survey), `timeoutSeconds: 0` disables the per-case timeout.

**`ctest` does nothing here.** `CMakeLists.txt` has no `enable_testing()` or `add_test()`, so there is
no registered suite despite `build.sh` exporting `CTEST_PARALLEL_LEVEL`. `./build.sh solved` is the
suite runner.

### C++ invariants worth knowing before editing build files

- **Profile name == CMake preset name == `build/` directory name.** `conanfile.py::_variant_tag()`
  composes it from compiler + `user.epi:variant` + build type, which is why the sanitizer profiles are
  named `gcc-asan-debug` rather than `gcc-asan`.
- **C++17, pinned in three places that must agree**: `compiler.cppstd=gnu17` in each
  `conan/profiles/*`, `CMAKE_CXX_STANDARD` in `conanfile.py::generate()`, and `set(CMAKE_CXX_STANDARD 17)`
  in `CMakeLists.txt`. A mismatch is silent — the plain `set()` shadows the toolchain value.
  `conanfile.py::validate()` enforces the floor.
- **`-Werror` is on** (`CMakeLists.txt`, non-MSVC branch), with `-Wno-unused-variable -Wno-sign-compare`.
  Any other new warning fails the build.
- A new problem needs its own `add_executable()` line in `CMakeLists.txt`; all 220 are listed there.
- Sanitizer flags use Conan's `&:` prefix so they scope to this package only and don't force a
  from-source rebuild of the dependency graph.
- Only `parity` links Tracy. `tracy/0.14.0` is pinned and comes from the local recipe in
  `conan/recipes/tracy` — it must match the wire protocol of the Tracy *server* installed on the
  machine, which hard-rejects a mismatch. `benchmark` and `gtest` are available but unwired.

## Python

```sh
cd epi_judge_python && python3 parity.py
```

Same harness, same TSV files, same cwd sensitivity. `generic_test.generic_test_main('parity.py',
'parity.tsv', func)` is the entry point, and the function argument is swapped the same way to compare
implementations.

## Java

```sh
cd epi_judge_java && make Parity     # compile and run one program
cd epi_judge_java && make            # the last-modified program
```

Builds into `java_build/`.

## Conventions in this fork

- **The stub marker is load-bearing.** `build.sh solved` derives its problem list from the *absence*
  of the literal string `// TODO - you fill in here.`, so bookkeeping stays automatic. A solved file
  that still carries the marker is silently excluded from `./build.sh solved` — strip it once the
  problem works.
- Some files split their contents with banner comments (`Programmer's Attempt` vs
  `Ai's optimized solutions`) to keep a hand-written attempt next to a reference implementation.
  Follow that layout when adding a reference version to a file that has one.
- Long-form reasoning for a problem goes in `solution-walkthrough.md`, keyed by book section number
  (e.g. `### 7.1 merge 2 sorted list`), not in the source file.
- Commit subjects are short and lowercase, often naming the problem: `solve 5.1,5.2`,
  `h_index: solved and tested`.

## Tooling that is deliberately absent

There is **no linter or formatter configured** anywhere in this tree — no `.clang-format`,
`.clang-tidy`, `pyproject.toml`, `.editorconfig` or requirements file. Match surrounding style by
hand; do not introduce a config without being asked. The only enforcement is the compiler:
`-Werror` on the C++ side.

`epijudge_ci_test_tool.py` at the root is upstream's CI driver (click-based, `stub`/`solution` modes
across all three languages). It backs `.github/workflows/`, which are restricted to the upstream repo
and do not run on this fork. It is not part of the local workflow — use `build.sh`.

## Parallelism

Builds are capped per host — compiling with all cores freezes these laptops. `build.sh` reads
`build_jobs` from the machine's inventory file (omarchy-tp → 6, omarchy-hpn → 2), falls back to
`nproc - 1`, and applies it to Conan, CMake and CTest alike. Override with `EPI_JOBS=N`.
`./build.sh all` is sequential for the same reason. `conan/profiles/base` hardcodes 6 as a safe
default only; `build.sh` overrides it per host.

`./build.sh solved` caps each problem at `EPI_RUN_TIMEOUT` (default 60s) so one looping solution
cannot wedge the suite — `kth_largest_in_array` currently loops.
