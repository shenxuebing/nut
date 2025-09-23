#!/usr/bin/env bash
set -euo pipefail

# Build on Linux: Debug & Release, SHARED and STATIC
# Output structure: build-linux/{Debug|Release}/{bin,lib}
# Usage: bash ./build-all-linux.sh

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

function do_config_build() {
  local build_type="$1"     # Debug or Release
  local lib_type="$2"       # SHARED or STATIC
  local bdir="${ROOT_DIR}/build-linux/${build_type}-${lib_type}"

  echo "==> Configure ${build_type} ${lib_type} to ${bdir}"
  cmake -S "${ROOT_DIR}" -B "${bdir}" -DCMAKE_BUILD_TYPE="${build_type}" -DNUT_LIBRARY_TYPE="${lib_type}" ${CMAKE_GENERATOR:+-G "${CMAKE_GENERATOR}"}

  echo "==> Build ${build_type} ${lib_type}"
  cmake --build "${bdir}" --config "${build_type}"

  echo "==> Artifacts (${build_type} ${lib_type}):"
  find "${bdir}" -type f \( -name "*.so" -o -name "*.a" -o -name "*.exe" -o -name "*.out" \) | sort
  
  # 验证库类型
  echo "==> Library Type Verification:"
  STATIC_LIB=$(find "${bdir}" -name "*.a" | head -1)
  SHARED_LIB=$(find "${bdir}" -name "*.so" | head -1)
  
  if [ "$lib_type" = "STATIC" ]; then
    if [ -n "$STATIC_LIB" ]; then
      echo "✅ Static library found: $STATIC_LIB"
      file "$STATIC_LIB"
    else
      echo "❌ No static library (.a) found!"
    fi
    if [ -n "$SHARED_LIB" ]; then
      echo "❌ Unexpected shared library found: $SHARED_LIB"
    else
      echo "✅ Correct: No shared library (.so) generated"
    fi
  else
    if [ -n "$SHARED_LIB" ]; then
      echo "✅ Shared library found: $SHARED_LIB"
      file "$SHARED_LIB"
    else
      echo "❌ No shared library (.so) found!"
    fi
    if [ -n "$STATIC_LIB" ]; then
      echo "❌ Unexpected static library found: $STATIC_LIB"
    else
      echo "✅ Correct: No static library (.a) generated"
    fi
  fi
  echo ""
}

# Optional: choose generator (uncomment if you prefer Ninja)
# export CMAKE_GENERATOR="Ninja"

# Clean previous linux builds
rm -rf "${ROOT_DIR}/build-linux" || true

# Debug/Release x SHARED/STATIC
do_config_build Debug SHARED
do_config_build Release SHARED
do_config_build Debug STATIC
do_config_build Release STATIC

echo "Done. See build-linux/{Debug-*,Release-*} for outputs."