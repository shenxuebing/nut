#!/usr/bin/env bash
set -euo pipefail

# Linux 静态库构建测试脚本
# 用于验证修复后的CMake配置是否正确生成静态库

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "=== Linux 静态库构建测试 ==="

# 清理之前的构建
rm -rf "${ROOT_DIR}/test-static-build" || true

# 测试静态库构建
echo "1. 构建静态库..."
cmake -S "${ROOT_DIR}" -B "${ROOT_DIR}/test-static-build" \
    -DCMAKE_BUILD_TYPE=Release \
    -DNUT_LIBRARY_TYPE=STATIC

cmake --build "${ROOT_DIR}/test-static-build" --config Release

echo "2. 检查生成的文件..."
echo "构建目录内容:"
find "${ROOT_DIR}/test-static-build" -type f \( -name "*.so" -o -name "*.a" -o -name "*.exe" \) | sort

echo ""
echo "3. 验证库文件类型..."
STATIC_LIB=$(find "${ROOT_DIR}/test-static-build" -name "*.a" | head -1)
SHARED_LIB=$(find "${ROOT_DIR}/test-static-build" -name "*.so" | head -1)

if [ -n "$STATIC_LIB" ]; then
    echo "✅ 找到静态库: $STATIC_LIB"
    file "$STATIC_LIB"
else
    echo "❌ 未找到静态库文件 (.a)"
fi

if [ -n "$SHARED_LIB" ]; then
    echo "❌ 意外找到动态库: $SHARED_LIB"
    file "$SHARED_LIB"
else
    echo "✅ 正确：未生成动态库文件 (.so)"
fi

echo ""
echo "4. 测试动态库构建..."
rm -rf "${ROOT_DIR}/test-shared-build" || true

cmake -S "${ROOT_DIR}" -B "${ROOT_DIR}/test-shared-build" \
    -DCMAKE_BUILD_TYPE=Release \
    -DNUT_LIBRARY_TYPE=SHARED

cmake --build "${ROOT_DIR}/test-shared-build" --config Release

echo "动态库构建结果:"
find "${ROOT_DIR}/test-shared-build" -type f \( -name "*.so" -o -name "*.a" -o -name "*.exe" \) | sort

echo ""
echo "=== 测试完成 ==="
echo "清理测试构建目录..."
rm -rf "${ROOT_DIR}/test-static-build" "${ROOT_DIR}/test-shared-build" || true
