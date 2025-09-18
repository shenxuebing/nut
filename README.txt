Nut C++ Utility Library
==========================

概述
----
Nut 是一个跨平台的 C/C++ 实用功能库，涵盖容器、日志、内存工具、数值算法、平台封装、并发、时间工具、安全/加密、单元测试框架、通用工具等模块，旨在提升工程开发效率与代码复用性。

主要特性与模块
--------------
- container  容器与数据结构：跳表、LRU 缓存、树结构、并发容器等
- logging    灵活的日志系统：多种 Handler（控制台、文件、轮转、syslog 等）
- mem       内存与对象管理：对象池、智能指针辅助、GC 辅助等
- memtool   内存操作与工具集：字节流、环形缓冲等
- numeric   数值算法：大整数、FFT/NTT、质数/模运算、校验/哈希（CRC/MD5/SHA）等
- platform  跨平台封装：OS/路径/系统信息等（Windows/Linux/macOS）
- rc        资源管理工具
- security  安全与加密：AES、RSA、DER/PEM 解析等
- threading 并发工具：线程池、锁、并发队列等
- time      时间工具：计时器、时间轮、时间差、性能计数器等
- unittest  简易单元测试框架
- util      常用工具：字符串、INI/XML DOM 等

目录结构
--------
- src/nut           库实现
- src/test_nut      单元测试/示例
- cmake             CMake 配置模板
- build             CMake 生成的构建产物目录（由脚本与配置生成）
- proj              其他工程模版（VS/Xcode/QtCreator 等）

构建要求
--------
- CMake 3.10+
- 编译器：
  - Windows: MSVC (Visual Studio 2019/2022)
  - Linux: GCC/Clang
- 可选：Ninja（Linux 下更快）

Windows 构建
------------
本仓库提供脚本 build-all.ps1，可一次构建：
- 架构：x64 与 Win32(x86)
- 运行时：MD 与 MT
- 类型：SHARED(动态库) 与 STATIC(静态库)
- 配置：Debug 与 Release

注意（Windows）：
- 产物目录：build\\{x64|Win32}\\{MD|MT}\\{bin,lib}\\{Debug|Release}
- 为避免与动态库 import lib 同名，静态库名追加后缀 "_static"（例如 nut_static.lib）
- 可执行 test_nut.exe 输出到对应 bin 目录

快速使用脚本：
1) 以管理员或普通 PowerShell 执行（建议 7+）：
   powershell -NoProfile -ExecutionPolicy Bypass -File .\\build-all.ps1
2) 产物清单将自动打印在末尾。

自定义一次性构建（示例）：
- 仅构建 x64 / MD / SHARED / Release：
  cmake -S . -B build -DBUILD_RUNTIME=MD -DNUT_LIBRARY_TYPE=SHARED
  cmake --build build --config Release

- 切换到 MT 或 STATIC：
  cmake -S . -B build -DBUILD_RUNTIME=MT -DNUT_LIBRARY_TYPE=STATIC
  cmake --build build --config Debug

Linux 构建
----------
本仓库提供脚本 build-all-linux.sh，构建 Debug/Release × SHARED/STATIC 的组合。
- 产物目录：build-linux/{Debug-*,Release-*}
- 输出目录结构（Linux）：{构建目录}/{bin,lib}

使用：
chmod +x ./build-all-linux.sh
bash ./build-all-linux.sh

可选：使用 Ninja 生成器（更快）：
export CMAKE_GENERATOR="Ninja"
bash ./build-all-linux.sh

也可手动构建（示例）：
mkdir -p build-linux/Debug-SHARED
cmake -S . -B build-linux/Debug-SHARED -DCMAKE_BUILD_TYPE=Debug -DNUT_LIBRARY_TYPE=SHARED
cmake --build build-linux/Debug-SHARED --config Debug

最小示例
--------
以 Windows x64 MD Release 为例，假设你已经生成了 nut.dll 与 nut.lib：
- 头文件包含路径（编译时）：
  - 源码包含：-I <repo_root>/src
- 链接库路径（链接时）：
  - -L <repo_root>/build/x64/MD/lib/Release
  - 链接库：nut.lib（动态库导入库）或 nut_static.lib（静态库）
- 运行时需要 nut.dll 位于可执行同目录或 PATH 中：
  - <repo_root>/build/x64/MD/bin/Release/nut.dll

示例代码（主干接口示意）：
/*
#include "nut/util/string_utils.h"
#include "nut/logging/logger.h"
#include "nut/time/date_time.h"

int main() {
    // 简单字符串工具
    auto s = nut::to_string(123);
    // 日志初始化与使用
    nut::Logger logger("demo");
    logger.info("hello nut, s={}", s);
    // 时间工具
    auto now = nut::DateTime::now();
    logger.info("now={}", now.to_string());
    return 0;
}
*/

CMake 最小示例（外部项目引入本库的思路）：
- 方法1：将本库作为子目录
  add_subdirectory(nut)  # 假设把仓库命名为 nut
  add_executable(app main.cpp)
  target_include_directories(app PUBLIC ${CMAKE_SOURCE_DIR}/nut/src)
  target_link_libraries(app PRIVATE nut)

- 方法2：使用已编译产物
  set(NUT_ROOT "E:/cpputil/nut-master/nut-master")
  set(NUT_INC  "${NUT_ROOT}/src")
  set(NUT_LIB  "${NUT_ROOT}/build/x64/MD/lib/Release")
  add_executable(app main.cpp)
  target_include_directories(app PUBLIC "${NUT_INC}")
  target_link_directories(app PRIVATE "${NUT_LIB}")
  target_link_libraries(app PRIVATE nut)   # 静态时用 nut_static

配置与选项
----------
- BUILD_RUNTIME (Windows only): MD 或 MT，决定输出目录顶层
- NUT_LIBRARY_TYPE: SHARED 或 STATIC，决定生成动态/静态库
- CMAKE_BUILD_TYPE (单配置生成器): Debug 或 Release
- MSVC 运行时：由 NUT_RUNTIME_LIBRARY 与 CMAKE_MSVC_RUNTIME_LIBRARY 控制（CMakeLists 已设置）

常见问题
--------
1) 动态库运行找不到 DLL
   - 将 nut.dll 拷贝到可执行同目录，或将 bin 目录加入 PATH

2) 静态与动态库重名冲突
   - Windows 已对静态库名追加 _static 后缀

3) 同时需要 MD 与 MT
   - 使用 build-all.ps1，它会在 build\\x64 与 build\\Win32 下分别生成 MD/MT

许可证与贡献
------------
- 许可证：BSD 3-Clause。允许在保留版权与免责声明的前提下，进行源代码和二进制形式的再分发与修改；不得未经许可使用版权方或贡献者名称为派生产品背书。完整文本见仓库根目录 LICENSE。
- 欢迎提交 Issue / PR 改进功能或文档

版本
----
- 当前版本：1.0.0（见 CMakeLists.txt）