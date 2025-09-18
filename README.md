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
- [七、许可证](#七许可证)

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
- **time**：时间工具：定时器、时间轮、性能计数器、时间差
- **unittest**：轻量单元测试框架
- **util**：常用工具：字符串、INI/XML DOM 等

## 二、环境与依赖
- **C++11 兼容编译器**（必需）
  - 本库基于 C++11 标准开发，需要完整支持 C++11 特性的编译器
- **CMake 3.10+**
- **编译器**
  - Windows：MSVC（Visual Studio 2019/2022）
  - Linux：GCC 4.8+ / Clang 3.3+（需支持 C++11）
  - macOS：Apple Clang（Xcode）
- **可选**：Ninja（Linux 推荐以提升构建速度）

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
- **生成**：
  ```cmd
  cmake -S . -B build -DBUILD_RUNTIME=MD -DNUT_LIBRARY_TYPE=SHARED
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
  cmake -S . -B build -DBUILD_RUNTIME=MT -DNUT_LIBRARY_TYPE=STATIC
  ```
- **构建**：
  ```cmd
  cmake --build build --config Debug
  ```
- **产物位置**：
  ```
  build\x64\MT\lib\Debug\nut_static.lib
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
1) **运行找不到 DLL（Windows）**
   - 将 `nut.dll` 放到可执行文件同目录，或将 bin 目录加入 PATH

2) **静态库与动态库同名冲突（Windows）**
   - 已自动对静态库追加 `_static` 后缀（`nut_static.lib`）

3) **同时需要 MD 与 MT？**
   - 使用 `build-all.ps1`，可在 x64 与 Win32 下分别生成 MD/MT 产物

4) **Linux 下的输出目录**
   - 使用独立构建目录（`build-linux/Debug-*`, `build-linux/Release-*`），避免覆盖

5) **编译器 C++11 兼容性**
   - Windows: VS2015 及以上版本支持 C++11
   - Linux: GCC 4.8+ 或 Clang 3.3+ 支持 C++11
   - macOS: 最新 Xcode 自带的 Clang 支持 C++11

## 七、许可证
- **许可证**：BSD 3-Clause
- **条款概述**：允许在保留版权与免责声明的前提下进行源代码和二进制形式的再分发与修改；不得未经许可使用版权方或贡献者名称为派生产品背书
- **完整文本**：见仓库根目录 LICENSE

## 鸣谢与贡献
- 欢迎提交 Issue / PR 改进功能或文档
- 任何建议都将帮助 Nut 变得更好
