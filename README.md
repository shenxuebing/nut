# Nut C++ Utility Library

## 简述
Nut 是一个现代、跨平台的 C/C++ 实用库，聚焦"工程常用能力"的高质量实现：容器/算法、日志、并发、时间、平台封装、加解密、单元测试与通用工具。目标是让通用能力"拿来就用"，减少重复造轮子。

## 特性亮点
- **丰富模块**：容器、日志、并发、时间、平台、数值、加密、安全、单元测试、通用工具等
- **跨平台**：Windows / Linux / macOS（已适配常见编译器）
- **C++11 标准**：基于 C++11 特性构建，确保现代性与兼容性
- **生产可用**：涵盖异常处理、日志、测试基建等工程必备
- **易集成**：CMake 驱动，提供快速构建脚本与最小示例
- **兼容性**：支持 MSVC 运行时切换（Windows 下 MD / MT）；静态库自动防重名（_static）

## 目录
- [一、模块与能力概览](#一模块与能力概览)
- [二、环境与依赖](#二环境与依赖)
- [三、快速开始（脚本一键构建）](#三快速开始脚本一键构建)
- [四、手动构建（CMake）](#四手动构建cmake)
- [五、最小示例（使用与链接）](#五最小示例使用与链接)
- [六、常见问题（FAQ）](#六常见问题faq)
- [七、最佳实践与调试技巧](#七最佳实践与调试技巧)
- [八、许可证](#八许可证)

## 一、模块与能力概览
- **container**：数据结构与容器：跳表、LRU 缓存、树结构、并发容器等
- **debugging**：调试与诊断工具：堆栈跟踪、增强断言、异常处理、源码位置信息、内存问题诊断等
- **logging**：日志系统：控制台 / 文件 / 轮转 / syslog 等 Handler
- **mem**：内存与对象管理：对象池、辅助 GC 等
- **memtool**：内存工具：字节流、环形缓冲等
- **numeric**：数值算法：大整数、FFT/NTT、质数/模运算、CRC/MD5/SHA 等
- **platform**：平台封装：OS / 路径 / 系统信息（Windows/Linux/macOS）
- **rc**：资源管理
- **security**：加解密：AES、RSA、DER/PEM 解析
- **threading**：并发工具：线程池、锁、并发队列等
- **time**：时间工具：定时器（支持立即退出）、时间轮、性能计数器、时间差
- **unittest**：轻量单元测试框架
- **util**：常用工具：字符串、INI/XML DOM 等

## 二、环境与依赖
- **C++11 兼容编译器**（必需）
  - 本库基于 C++11 标准开发，需要完整支持 C++11 特性的编译器
- **CMake 3.10+**
- **编译器**
  - Windows：MSVC（Visual Studio 2019/2022/2026）
  - Linux：GCC 4.8+ / Clang 3.3+（需支持 C++11）
  - macOS：Apple Clang（Xcode）
- **可选**：Ninja（推荐以提升构建速度）

### Windows 编译器兼容性
- **Visual Studio 2022** (MSVC 19.44.x) - 推荐版本
- **Visual Studio 2026** (MSVC 19.50.x) - 预览版本支持
- **Visual Studio 2019** (MSVC 19.29.x) - 基础支持

### 运行时库配置
- **动态运行时**：`/MD` (Release) / `/MDd` (Debug)
- **静态运行时**：`/MT` (Release) / `/MTd` (Debug)
- **自动配置**：通过 `STATIC_CRT` 选项控制
  - `STATIC_CRT=ON` → 使用静态运行时库
  - `STATIC_CRT=OFF` → 使用动态运行时库

## 三、快速开始（脚本一键构建）
### Windows（同时构建 x64 与 Win32 全矩阵）
- **产物目录**（示例）：
  - `build\x64\{MD|MT}\{bin,lib}\{Debug|Release}`
  - `build\Win32\{MD|MT}\{bin,lib}\{Debug|Release}`
- **运行**：
  ```powershell
  powershell -NoProfile -ExecutionPolicy Bypass -File .\build-all.ps1
  ```
- **说明**：
  - 支持 MD/MT × SHARED/STATIC × Debug/Release
  - 为避免与动态库 import lib 重名，静态库名自动追加后缀：`nut_static.lib`
  - 编译器需支持 C++11 标准

### Linux（Debug/Release × 动/静态库）
- **产物目录**（示例）：`build-linux/{Debug-*,Release-*}/{bin,lib}`
- **运行**：
  ```bash
  chmod +x ./build-all-linux.sh
  bash ./build-all-linux.sh
  ```
- **可选**：使用 Ninja（更快）
  ```bash
  export CMAKE_GENERATOR="Ninja"
  bash ./build-all-linux.sh
  ```
- **注意**：确保 GCC/Clang 版本支持完整 C++11 特性

## 四、手动构建（CMake）
### Windows 示例（x64 / MD / SHARED / Release）
- **使用 Visual Studio 生成器**：
  ```cmd
  cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DBUILD_RUNTIME=MD -DNUT_LIBRARY_TYPE=SHARED
  ```
- **使用 Ninja 生成器（推荐）**：
  ```cmd
  cmake -S . -B build -G Ninja -DBUILD_RUNTIME=MD -DNUT_LIBRARY_TYPE=SHARED -DCMAKE_BUILD_TYPE=Release
  ```
- **构建**：
  ```cmd
  cmake --build build --config Release
  ```
- **产物位置**：
  ```
  build\x64\MD\bin\Release\nut.dll
  build\x64\MD\lib\Release\nut.lib
  ```

### 切换到 MT 或 STATIC
- **生成**：
  ```cmd
  cmake -S . -B build -G Ninja -DBUILD_RUNTIME=MT -DNUT_LIBRARY_TYPE=STATIC -DSTATIC_CRT=ON -DCMAKE_BUILD_TYPE=Debug
  ```
- **构建**：
  ```cmd
  cmake --build build --config Debug
  ```
- **产物位置**：
  ```
  build\x64\MT\lib\Debug\nut_static.lib
  ```

### CMake 配置选项详解
| 选项 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `BUILD_RUNTIME` | STRING | `MT` | 运行时库类型：`MT`(静态) 或 `MD`(动态) |
| `NUT_RUNTIME_LIBRARY` | STRING | `MT` | 运行时库类型（向后兼容） |
| `NUT_LIBRARY_TYPE` | STRING | `SHARED` | 库类型：`STATIC` 或 `SHARED` |
| `STATIC_CRT` | BOOL | `OFF` | 是否使用静态运行时库 |
| `BUILD_SHARED_LIBS` | BOOL | `ON` | 是否构建共享库 |
| `BUILD_STATIC_LIBS` | BOOL | `OFF` | 是否构建静态库 |

### 多配置构建（Visual Studio）
```cmd
# 生成支持多配置的项目
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DBUILD_RUNTIME=MT -DNUT_LIBRARY_TYPE=STATIC -DSTATIC_CRT=ON

# 分别构建 Debug 和 Release
cmake --build build --config Debug
cmake --build build --config Release
```

### Linux 示例（Debug / SHARED）
- **生成**：
  ```bash
  mkdir -p build-linux/Debug-SHARED
  cmake -S . -B build-linux/Debug-SHARED -DCMAKE_BUILD_TYPE=Debug -DNUT_LIBRARY_TYPE=SHARED
  ```
- **构建**：
  ```bash
  cmake --build build-linux/Debug-SHARED --config Debug
  ```

## 五、最小示例（使用与链接）
以 Windows x64 MD Release 为例：

### 包含与链接
- **头文件包含路径**：
  - `-I <repo_root>/src`
- **库路径与库名**：
  - `-L <repo_root>/build/x64/MD/lib/Release`
  - 链接 `nut.lib`（动态库导入库）或 `nut_static.lib`（静态库）
- **运行时**：
  - 请确保 `nut.dll` 位于可执行文件同目录，或已加入 PATH

### 示例代码（简要）
```cpp
#include "nut/util/string_utils.h"
#include "nut/logging/logger.h"
#include "nut/time/date_time.h"

int main() {
    auto s = nut::to_string(123);
    nut::Logger logger("demo");
    logger.info("hello nut, s={}", s);
    auto now = nut::DateTime::now();
    logger.info("now={}", now.to_string());
    return 0;
}
```

### CMake 集成（两种方式）
- **方式一**：子目录集成
  ```cmake
  add_subdirectory(nut)             # 假设当前仓库目录名为 nut
  add_executable(app main.cpp)
  target_include_directories(app PUBLIC ${CMAKE_SOURCE_DIR}/nut/src)
  target_link_libraries(app PRIVATE nut)
  ```

- **方式二**：链接已编译产物
  ```cmake
  set(NUT_ROOT "E:/cpputil/nut-master/nut-master")
  set(NUT_INC  "${NUT_ROOT}/src")
  set(NUT_LIB  "${NUT_ROOT}/build/x64/MD/lib/Release")
  add_executable(app main.cpp)
  target_include_directories(app PUBLIC "${NUT_INC}")
  target_link_directories(app PRIVATE "${NUT_LIB}")
  target_link_libraries(app PRIVATE nut)   # 静态库用 nut_static
  ```

## 六、常见问题（FAQ）

### 1) 运行找不到 DLL（Windows）
- 将 `nut.dll` 放到可执行文件同目录，或将 bin 目录加入 PATH

### 2) 静态库与动态库同名冲突（Windows）
- 已自动对静态库追加 `_static` 后缀（`nut_static.lib`）

### 3) 同时需要 MD 与 MT？
- 使用 `build-all.ps1`，可在 x64 与 Win32 下分别生成 MD/MT 产物

### 4) Linux 下的输出目录
- 使用独立构建目录（`build-linux/Debug-*`, `build-linux/Release-*`），避免覆盖

### 5) 编译器 C++11 兼容性
- Windows: VS2015 及以上版本支持 C++11
- Linux: GCC 4.8+ 或 Clang 3.3+ 支持 C++11
- macOS: 最新 Xcode 自带的 Clang 支持 C++11

### 6) 运行时库不匹配错误
**症状**：
```
error LNK2038: 检测到"RuntimeLibrary"的不匹配项: 值"MTd_StaticDebug"不匹配值"MT_StaticRelease"
```

**解决方案**：
- 确保所有库和可执行文件使用相同的运行时库配置
- 检查 `STATIC_CRT` 选项设置是否一致
- 使用生成器表达式：`MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"`

### 7) _ITERATOR_DEBUG_LEVEL 不匹配
**症状**：
```
error LNK2038: 检测到"_ITERATOR_DEBUG_LEVEL"的不匹配项: 值"2"不匹配值"0"
```

**解决方案**：
- 确保所有目标在 Debug 模式下都使用相同的运行时库
- 检查 CMake 配置中的 `CMAKE_BUILD_TYPE` 设置
- 对于 Ninja 生成器，明确指定 `-DCMAKE_BUILD_TYPE=Debug`

### 8) CMake 生成器不匹配
**症状**：
```
CMake Error: Error: generator : Visual Studio 17 2022 Does not match the generator used previously: Ninja
```

**解决方案**：
- 清理构建目录：`rm -rf build/*` 或 `rmdir /s /q build`
- 重新运行 CMake 配置

### 9) 编译器版本不匹配
**症状**：
- 链接时出现未解析的外部符号
- 运行时库函数找不到

**解决方案**：
- 确保所有库使用相同版本的编译器编译
- 检查 `CMAKE_CXX_COMPILER` 和 `CMAKE_C_COMPILER` 设置
- 在 Windows 上，统一使用 Visual Studio 2022 或 2026

### 10) Ninja 生成器相关问题
**症状**：
```
CMake Error: CMake was unable to find a build program corresponding to "Ninja"
```

**解决方案**：
- 安装 Ninja：`pip install ninja` 或使用 Visual Studio 自带的 Ninja
- 指定 Ninja 路径：`-DCMAKE_MAKE_PROGRAM=<path_to_ninja>`

### 11) 调试信息验证
**如何确认库使用了正确的运行时库**：
```cmd
# 检查符号表
dumpbin /symbols nut_static.lib | findstr __malloc_dbg
# 如果看到 __malloc_dbg 符号，说明使用了 Debug 运行时库 (/MTd)

# 检查编译选项
grep -r "MTd" build.ninja
```

### 12) CMakeSettings.json 配置示例
```json
{
  "configurations": [
    {
      "name": "x86-Debug",
      "generator": "Ninja",
      "configurationType": "Debug",
      "buildRoot": "${projectDir}\\out\\build\\${name}",
      "inheritEnvironments": [ "msvc_x86" ],
      "variables": [
        {
          "name": "BUILD_RUNTIME",
          "value": "MT",
          "type": "STRING"
        },
        {
          "name": "STATIC_CRT",
          "value": "True",
          "type": "BOOL"
        }
      ]
    }
  ]
}
```

## 七、最佳实践与调试技巧

### 7.1 构建配置最佳实践
1. **统一编译器版本**：确保所有依赖库使用相同版本的编译器
2. **统一运行时库**：在同一个项目中，所有目标应使用相同的运行时库配置
3. **使用 Ninja 生成器**：相比 Visual Studio 生成器，Ninja 更快且配置更直观
4. **明确指定构建类型**：使用 `-DCMAKE_BUILD_TYPE=Debug/Release` 避免默认值问题

### 7.2 常见构建命令速查
```bash
# Windows x86 Debug 静态库
cmake -G Ninja -DBUILD_RUNTIME=MT -DNUT_LIBRARY_TYPE=STATIC -DSTATIC_CRT=ON -DCMAKE_BUILD_TYPE=Debug -S . -B out/build/x86-Debug

# Windows x64 Release 动态库
cmake -G Ninja -DBUILD_RUNTIME=MD -DNUT_LIBRARY_TYPE=SHARED -DSTATIC_CRT=OFF -DCMAKE_BUILD_TYPE=Release -S . -B out/build/x64-Release

# Linux Debug 静态库
cmake -G Ninja -DNUT_LIBRARY_TYPE=STATIC -DCMAKE_BUILD_TYPE=Debug -S . -B build-linux/Debug-STATIC
```

### 7.3 调试运行时库问题
当遇到运行时库不匹配时，按以下步骤排查：

1. **检查编译选项**：
   ```cmd
   grep -r "MTd\|MDd" build.ninja
   ```

2. **检查符号表**：
   ```cmd
   dumpbin /symbols nut_static.lib | findstr __malloc_dbg
   # Debug 库应包含 __malloc_dbg, __free_dbg 等符号
   ```

3. **检查 CMake 配置**：
   ```cmd
   grep -r "STATIC_CRT\|BUILD_RUNTIME" CMakeCache.txt
   ```

### 7.4 多项目集成建议
当 nut 库被其他项目使用时：

1. **提供预编译库**：为常见配置提供预编译的二进制包
2. **CMake 配置模板**：提供标准的 CMakeLists.txt 模板
3. **版本兼容性**：明确标注支持的编译器版本和运行时库配置

### 7.5 性能优化建议
1. **发布版本**：使用 Release 配置和优化选项
2. **并行编译**：使用 Ninja 的并行编译能力
3. **链接时优化**：在 Release 模式下启用 LTO（Link Time Optimization）

## 八、许可证
- **许可证**：BSD 3-Clause
- **条款概述**：允许在保留版权与免责声明的前提下进行源代码和二进制形式的再分发与修改；不得未经许可使用版权方或贡献者名称为派生产品背书
- **完整文本**：见仓库根目录 LICENSE

## 鸣谢与贡献
- 本项目原仓库地址：[https://github.com/jingqi/nut](https://github.com/jingqi/nut)，因原仓库长期未更新，现已独立维护。
- 欢迎提交 Issue / PR 改进功能或文档
- 任何建议都将帮助 Nut 变得更好
