#!/usr/bin/env bash
set -euo pipefail

# Build bundled libiconv and nut on Linux:
#   Debug/Release x SHARED/STATIC
# Output structure: build-linux/{Debug|Release}-{SHARED|STATIC}
# Usage: bash ./build-all-linux.sh

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_ROOT="${ROOT_DIR}/build-linux"

if [[ "$(uname -s)" != "Linux" ]]; then
  echo "This script must be run on Linux (detected: $(uname -s))." >&2
  exit 1
fi

command -v cmake >/dev/null 2>&1 || { echo "cmake is required." >&2; exit 1; }
command -v make >/dev/null 2>&1 || command -v ninja >/dev/null 2>&1 || {
  echo "make or ninja is required." >&2
  exit 1
}

if [[ ! -f "${ROOT_DIR}/third_party/libiconv/CMakeLists.txt" ]]; then
  echo "Bundled libiconv source is missing: ${ROOT_DIR}/third_party/libiconv" >&2
  exit 1
fi

if [[ -n "${CMAKE_GENERATOR:-}" ]]; then
  CMAKE_GENERATOR_ARGS=(-G "${CMAKE_GENERATOR}")
else
  CMAKE_GENERATOR_ARGS=()
fi

if [[ -e "${BUILD_ROOT}" ]]; then
  resolved_build_root="$(readlink -f "${BUILD_ROOT}")"
  resolved_root="$(readlink -f "${ROOT_DIR}")"
  [[ "${resolved_build_root}" == "${resolved_root}/build-linux" ]] || {
    echo "Refusing to clean unexpected path: ${resolved_build_root}" >&2
    exit 1
  }
  echo "==> Cleaning ${BUILD_ROOT}"
  rm -rf -- "${BUILD_ROOT}"
fi

do_config_build() {
  local build_type="$1"
  local lib_type="$2"
  local bdir="${BUILD_ROOT}/${build_type}-${lib_type}"

  echo "==> Configure ${build_type} ${lib_type}"
  if [[ -n "${CMAKE_GENERATOR:-}" ]]; then
    cmake -S "${ROOT_DIR}" -B "${bdir}" -G "${CMAKE_GENERATOR}" \
      -DCMAKE_BUILD_TYPE="${build_type}" \
      -DNUT_LIBRARY_TYPE="${lib_type}"
  else
    cmake -S "${ROOT_DIR}" -B "${bdir}" \
      -DCMAKE_BUILD_TYPE="${build_type}" \
      -DNUT_LIBRARY_TYPE="${lib_type}"
  fi

  echo "==> Build ${build_type} ${lib_type}"
  if [[ "${lib_type}" == "STATIC" ]]; then
    cmake --build "${bdir}" --target nut_static --parallel
  else
    cmake --build "${bdir}" --target nut_shared --parallel
  fi

  local nut_static="${bdir}/lib/libnut_static.a"
  local nut_shared="${bdir}/lib/libnut.so"

  echo "==> Verify ${build_type} ${lib_type}"
  if [[ "${lib_type}" == "STATIC" ]]; then
    [[ -f "${nut_static}" ]] || { echo "Missing ${nut_static}" >&2; exit 1; }
    [[ ! -f "${nut_shared}" ]] || { echo "Unexpected ${nut_shared}" >&2; exit 1; }
    echo "Static nut: ${nut_static}"
  else
    [[ -f "${nut_shared}" ]] || { echo "Missing ${nut_shared}" >&2; exit 1; }
    [[ ! -f "${nut_static}" ]] || { echo "Unexpected ${nut_static}" >&2; exit 1; }
    echo "Shared nut: ${nut_shared}"
  fi

  if command -v file >/dev/null 2>&1; then
    [[ "${lib_type}" == "STATIC" ]] && file "${nut_static}" || file "${nut_shared}"
  fi
  echo
}

do_config_build Debug SHARED
do_config_build Release SHARED
do_config_build Debug STATIC
do_config_build Release STATIC

echo "Done. See ${BUILD_ROOT}/{Debug-*,Release-*} for outputs."
