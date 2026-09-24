# EPIJudge C++ — build, run, debug, profile

Working notes for this fork. The repo-root `README.md` is upstream's and describes a
`g++ -std=c++14` Makefile flow that predates all of this — prefer what's here.

Everything below runs from `epi_judge_cpp/`.

## Layout

```
epi_judge_cpp/
├── *.cc                    220 problems, one main() each; target name == basename
├── test_framework/         header-only judge harness (shared by every problem)
├── conanfile.py            dependency + toolchain definition
├── conan/profiles/         checked-in toolchain profiles  <- the source of truth
├── build.sh                driver; wraps conan + cmake presets
└── build/<toolchain>/      one tree per toolchain, e.g. build/gcc-debug/
```

Test fixtures live in `../test_data/<problem>.tsv`, and `../config.json` holds the
harness's timeout and stop-on-failure settings.

## Toolchains

| Profile | Compiler | Standard library | Build type |
|---|---|---|---|
| `gcc-debug` / `gcc-release` | gcc 16 | libstdc++11 | Debug / Release |
| `clang-debug` / `clang-release` | clang 22 | **libc++** | Debug / Release |
| `gcc-asan-debug` | gcc 16 | libstdc++11 | Debug + ASan/UBSan |
| `gcc-tsan-debug` | gcc 16 | libstdc++11 | Debug + TSan |

Two standard libraries on purpose: libc++ catches non-portable libstdc++ assumptions
that gcc never flags. Each profile gets its own `build/` tree and its own CMake preset
of the same name, so they never clash.

ConanCenter publishes Linux binaries for gcc 13 / gnu17 only, so at gcc 16 / clang 22
**every dependency is built from source**. The first `setup` of each profile is slow;
afterwards it's cached.

## First-time setup

```bash
./build.sh                      # setup + configure + build, gcc-debug
```

Or step by step, for any toolchain:

```bash
./build.sh setup     gcc-debug  # conan install --build=missing
./build.sh configure gcc-debug  # cmake --preset gcc-debug
./build.sh build     gcc-debug  # all 220 targets
```

## Everyday use

```bash
./build.sh build gcc-debug parity      # one target only
./build.sh run   gcc-debug parity      # build/gcc-debug/parity, correct cwd + flags
./build.sh solved gcc-debug            # run every solved problem, report pass/fail
./build.sh all                         # all four toolchains, sequentially
./build.sh compdb gcc-debug            # point clangd at this toolchain
./build.sh clean
```

`solved` derives the problem list by looking for the absence of the upstream stub
marker `// TODO - you fill in here.`, so it picks up newly solved problems with no
bookkeeping. Each problem is capped by `EPI_RUN_TIMEOUT` (default 60s) so one looping
solution can't wedge the run.

## Two traps when running a binary by hand

Both are properties of the upstream harness, and `build.sh run` handles them for you.

**1. The working directory matters more than `--test-data-dir`.**
`GenericTestMain` loads `config.json` through `GetDefaultTestDataDirPath()`
(`test_framework/test_utils.h`), which walks **at most three** parent directories
looking for `test_data/` — and it does so *before* `--test-data-dir` is parsed, outside
any try/catch. From the wrong directory the binary aborts with
`Can't find test data directory` no matter what flags you pass.

This is why the build trees are `build/<toolchain>/` and not deeper: at that depth it
resolves, so `./build/gcc-debug/lru_cache` just works. (The previous doubled
`build/build/Release/` layout was one level too deep, which is why nothing could be run
from its own build directory.)

**2. Every run rewrites `problem_mapping.js` unless you opt out.**
`update_js` defaults to true (`test_framework/test_config.h`), so a plain run dirties
your git tree. Pass `--no-update-js`.

Putting both together, the manual equivalent of `./build.sh run`:

```bash
cd ..   # the judge root, where test_data/ lives
./epi_judge_cpp/build/gcc-debug/lru_cache --test-data-dir test_data --no-update-js
```

Other useful harness flags: `--no-tty --no-color` (clean logs), `-h` for the rest.

## Debugging

```bash
./build.sh build gcc-debug lru_cache
cd .. && gdb --args ./epi_judge_cpp/build/gcc-debug/lru_cache \
    --test-data-dir test_data --no-update-js
```

Run gdb from the judge root for the reason above. gdb 17.2 is the only debugger
installed; `lldb`, `valgrind`, `perf`, `heaptrack` and `rr` are not (`extra/lldb`,
`extra/valgrind`, `extra/linux-tools`, `extra/heaptrack`, `extra/rr` on Arch).

### Sanitizers

The practical substitute for the missing valgrind, and far faster (~2x overhead versus
~20x):

```bash
./build.sh build gcc-asan-debug        # ASan + UBSan, build/gcc-asan-debug/
./build.sh solved gcc-asan-debug
./build.sh build gcc-tsan-debug        # ThreadSanitizer (cannot combine with ASan)
```

The sanitizer flags are scoped to this package with Conan's `&:` prefix, so enabling
them never rebuilds the dependency graph. If the linked-list problems' intentional
leaks get noisy: `ASAN_OPTIONS=detect_leaks=0`.

## Profiling with Tracy

`parity` is the instrumented target (`ZoneScoped` in `main`). Build it and run the
Tracy profiler UI to connect. Note the judge binaries finish in milliseconds, so start
the profiler first or the capture window is missed.

## clangd

```bash
./build.sh compdb gcc-debug
```

symlinks `compile_commands.json` to the chosen toolchain's. Switch it to
`clang-debug` when you want clangd to see libc++ headers rather than libstdc++.
Both the symlink and `.cache/` are gitignored.

## Parallelism

Builds here are capped per host — building with all cores freezes these laptops.
`build.sh` reads `build_jobs` from the machine's inventory file (omarchy-tp → 6,
omarchy-hpn → 2), falling back to `nproc - 1`, and applies it to Conan, CMake and
CTest alike. Override with `EPI_JOBS=N`. `./build.sh all` runs toolchains one after
another for the same reason.

## Changing dependencies

`conanfile.py` declares `benchmark`, `gtest` and `tracy`. Only Tracy is wired into a
target; the other two are available for when you want to write a unit test or a
microbenchmark. boost and yaml-cpp were removed — nothing included them, and each
unused dependency was a from-source build for nothing on this toolchain. To add one
back, add the `requires()` line plus the matching `find_package()` /
`target_link_libraries()` in `CMakeLists.txt`.

Bumping a compiler? Update `compiler.version` in the relevant `conan/profiles/*` file;
`conanfile.py`'s `validate()` enforces the C++17 floor.
