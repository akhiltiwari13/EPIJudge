#!/usr/bin/env bash
#
# Build driver for the EPIJudge C++ problem set.
#
# Wraps the Conan 2 + CMake preset flow for the toolchains defined in
# conan/profiles/. Every parallelism channel is capped per-host, because
# building with all cores freezes these laptops.
#
#   ./build.sh                       # setup + configure + build, gcc-debug
#   ./build.sh build clang-release
#   ./build.sh build gcc-debug parity   # one target
#   ./build.sh run gcc-debug lru_cache
#   ./build.sh solved gcc-debug      # run the solved problems
#   ./build.sh all                   # all four toolchains, sequentially
#   ./build.sh compdb gcc-debug      # point clangd at this build
#   ./build.sh clean

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
JUDGE_ROOT="$(cd -- "${SCRIPT_DIR}/.." && pwd)"
cd "${SCRIPT_DIR}"

TOOLCHAINS=(gcc-debug gcc-release clang-debug clang-release)

# ---------------------------------------------------------------- job cap ---
# Per-host, never `nproc`. The inventory file is authoritative when present
# (omarchy-tp -> 6, omarchy-hpn -> 2); otherwise leave one thread free.
derive_jobs() {
  if [[ -n "${EPI_JOBS:-}" ]]; then echo "${EPI_JOBS}"; return; fi
  local facts="${HOME}/Work/projects/quomptrade/configfiles/inventory/$(cat /etc/hostname 2>/dev/null).facts"
  local n
  n="$(grep -oP 'build_jobs=\K[0-9]+' "${facts}" 2>/dev/null || true)"
  if [[ -z "${n}" ]]; then n=$(( $(nproc) - 1 )); fi
  (( n < 1 )) && n=1
  echo "${n}"
}
JOBS="$(derive_jobs)"
export CMAKE_BUILD_PARALLEL_LEVEL="${JOBS}"
export CTEST_PARALLEL_LEVEL="${JOBS}"

# The 24 problems that are actually solved are those without the upstream stub
# marker -- derived, not hard-coded, so this stays correct as you solve more.
solved_problems() {
  grep -LF '// TODO - you fill in here.' ./*.cc | xargs -n1 basename | sed 's/\.cc$//'
}

require_toolchain() {
  local tc="${1}"
  [[ -f "conan/profiles/${tc}" ]] || {
    echo "unknown toolchain '${tc}'; available: $(ls conan/profiles | grep -v '^base$' | tr '\n' ' ')" >&2
    exit 2
  }
}

# INVARIANT: profile name == CMake preset name == build/<dir> name.
# conanfile.py builds the preset prefix from compiler + user.epi:variant and
# appends the build type, so a profile must be named to match -- which is why
# the sanitizer profiles are gcc-asan-debug, not gcc-asan.
# tracy 0.14.0 is not on ConanCenter; it has to match the locally installed
# tracy-profiler's wire protocol. Exporting is idempotent and takes <1s.
export_local_recipes() {
  conan export conan/recipes/tracy --version 0.14.0 >/dev/null
}

do_setup() {
  local tc="${1}"; require_toolchain "${tc}"
  export_local_recipes
  echo ">>> conan install (${tc}, ${JOBS} jobs)"
  conan install . -pr:a "conan/profiles/${tc}" \
    -c "tools.build:jobs=${JOBS}" --build=missing
}

do_configure() {
  local tc="${1}"; shift; require_toolchain "${tc}"
  echo ">>> cmake configure (${tc})"
  cmake --preset "${tc}" "$@"
}

do_build() {
  local tc="${1}"; shift; require_toolchain "${tc}"
  echo ">>> cmake build (${tc}, ${JOBS} jobs)"
  if [[ $# -gt 0 ]]; then
    cmake --build --preset "${tc}" -j "${JOBS}" --target "$@"
  else
    cmake --build --preset "${tc}" -j "${JOBS}"
  fi
}

# Judge binaries must run with the judge root reachable within 3 parent dirs,
# and --no-update-js keeps them from rewriting problem_mapping.js on every run.
do_run() {
  local tc="${1}" prob="${2}"; require_toolchain "${tc}"
  local bin="${SCRIPT_DIR}/build/${tc}/${prob}"
  [[ -x "${bin}" ]] || { echo "not built: ${bin}" >&2; exit 2; }
  ( cd "${JUDGE_ROOT}" && "${bin}" --test-data-dir test_data --no-update-js "${@:3}" )
}

# Per-problem wall-clock cap. A problem whose solution is mid-refactor can
# loop forever (kth_largest_in_array does today), and without this one bad
# problem wedges the whole suite. Debug builds are unoptimised, so keep it
# generous.
EPI_RUN_TIMEOUT="${EPI_RUN_TIMEOUT:-60}"

do_solved() {
  local tc="${1}"; require_toolchain "${tc}"
  local pass=0 fail=0 timedout=0 missing=0 rc
  for p in $(solved_problems); do
    local bin="${SCRIPT_DIR}/build/${tc}/${p}"
    if [[ ! -x "${bin}" ]]; then
      printf '  %-40s NOT BUILT\n' "${p}"; missing=$(( missing + 1 )); continue
    fi
    rc=0
    ( cd "${JUDGE_ROOT}" && timeout "${EPI_RUN_TIMEOUT}" "${bin}" \
        --test-data-dir test_data --no-update-js --no-tty --no-color \
        >/dev/null 2>&1 ) || rc=$?
    case "${rc}" in
      0)   printf '  %-40s PASS\n'    "${p}"; pass=$(( pass + 1 )) ;;
      124) printf '  %-40s TIMEOUT (%ss)\n' "${p}" "${EPI_RUN_TIMEOUT}"
           timedout=$(( timedout + 1 )) ;;
      *)   printf '  %-40s FAIL (rc=%s)\n' "${p}" "${rc}"; fail=$(( fail + 1 )) ;;
    esac
  done
  echo "  ${tc}: ${pass} passed, ${fail} failed, ${timedout} timed out, ${missing} not built"
  (( fail == 0 && timedout == 0 && missing == 0 ))
}

do_compdb() {
  local tc="${1}"; require_toolchain "${tc}"
  ln -sfn "build/${tc}/compile_commands.json" compile_commands.json
  echo "compile_commands.json -> build/${tc}/compile_commands.json"
}

usage() { sed -n '3,20p' "${BASH_SOURCE[0]}" | sed 's/^# \?//'; }

cmd="${1:-default}"
case "${cmd}" in
  default)   do_setup gcc-debug; do_configure gcc-debug; do_build gcc-debug ;;
  setup)     do_setup "${2:-gcc-debug}" ;;
  configure) do_configure "${2:-gcc-debug}" "${@:3}" ;;
  build)     tc="${2:-gcc-debug}"; do_setup "${tc}"; do_configure "${tc}"; do_build "${tc}" "${@:3}" ;;
  run)       do_run "${2:?toolchain}" "${3:?problem}" "${@:4}" ;;
  solved)    do_solved "${2:-gcc-debug}" ;;
  compdb)    do_compdb "${2:-gcc-debug}" ;;
  clean)     rm -rf build CMakeUserPresets.json compile_commands.json; echo "cleaned" ;;
  all)
    # Sequential on purpose: four toolchains in parallel would be 4 x JOBS.
    rc=0
    for tc in "${TOOLCHAINS[@]}"; do
      echo "=============================== ${tc} ==============================="
      do_setup "${tc}"; do_configure "${tc}"; do_build "${tc}"
      do_solved "${tc}" || rc=1
    done
    exit "${rc}" ;;
  -h|--help|help) usage ;;
  *) echo "unknown command '${cmd}'" >&2; usage >&2; exit 2 ;;
esac
