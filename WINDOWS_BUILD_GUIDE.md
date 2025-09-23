# NUT Library Windows 构建指南

## 概述

NUT库现在完全支持Windows平台的CMake构建，包括：

- ✅ **UTF-8编码支持** - 自动添加 `/utf-8` 编译选项
- ✅ **MT/MD运行时库支持** - 支持静态和动态运行时库
- ✅ **静态/动态库支持** - 可生成.lib静态库和.dll动态库
- ✅ **多配置输出** - 不同配置输出到不同目录
- ✅ **便捷构建脚本** - Python和批处理脚本支持

## 目录结构

Windows构建会按照以下结构组织输出文件：

```
build/
├── MD/                     # MD运行时库
│   ├── shared/            # 动态库 (DLL)
│   │   ├── bin/
│   │   │   └── Debug/     # nut.dll + test_nut.exe
│   │   └── lib/
│   │       └── Debug/     # nut.lib (导入库)
│   └── static/            # 静态库
│       └── lib/
│           └── Debug/     # nut.lib (静态库)
└── MT/                     # MT运行时库
    ├── shared/            # 动态库 (DLL)
    │   ├── bin/
    │   │   └── Debug/     # nut.dll + test_nut.exe
    │   └── lib/
    │       └── Debug/     # nut.lib (导入库)
    └── static/            # 静态库
        └── lib/
            └── Debug/     # nut_static.lib (静态库)
```

## 构建选项

### 运行时库类型
- **MD** (默认): 多线程DLL运行时库 (/MD, /MDd)
- **MT**: 多线程静态运行时库 (/MT, /MTd)

### 库类型
- **SHARED** (默认): 动态链接库 (.dll + .lib导入库)
- **STATIC**: 静态链接库 (.lib)

## 使用方法

### 1. Python构建脚本（推荐）

```batch
# 默认构建 (MD + SHARED)
python build.py

# 构建MT静态库
python build.py --runtime-lib MT --lib-type STATIC

# 构建MD静态库
python build.py --lib-type STATIC

# 构建MT动态库
python build.py --runtime-lib MT

# 构建所有组合
python build.py --build-all

# 仅配置不构建
python build.py configure --runtime-lib MT --lib-type STATIC

# 显示帮助
python build.py --help
```

### 2. Windows批处理脚本

```batch
# 默认构建 (MD + SHARED)
build.bat

# 构建MT静态库
build.bat --runtime-lib MT --lib-type STATIC

# 构建所有组合
build.bat --build-all

# 显示帮助
build.bat --help
```

### 3. 直接使用CMake

```batch
# 配置MD动态库（默认）
cmake -DCMAKE_BUILD_TYPE=Release -B build

# 配置MT静态库
cmake -DCMAKE_BUILD_TYPE=Release -DNUT_RUNTIME_LIBRARY=MT -DNUT_LIBRARY_TYPE=STATIC -B build

# 构建
cmake --build build --config Release
```

## CMake选项说明

| 选项 | 值 | 说明 |
|------|-----|------|
| `NUT_RUNTIME_LIBRARY` | `MD` (默认) / `MT` | 运行时库类型 |
| `NUT_LIBRARY_TYPE` | `SHARED` (默认) / `STATIC` | 库类型 |
| `CMAKE_BUILD_TYPE` | `Release` (默认) / `Debug` / `RelWithDebInfo` / `MinSizeRel` | 构建类型 |
| `INSTALL_TESTS` | `OFF` (默认) / `ON` | 是否安装测试程序 |

## 构建结果

### MD动态库
- **nut.dll** (~1.8MB) - 动态链接库
- **nut.lib** (~483KB) - 导入库
- **nut.pdb** (~8MB) - 调试信息

### MT静态库  
- **nut_static.lib** (~23MB) - 静态链接库
- **nut_static.pdb** (~1.8MB) - 调试信息

## 在项目中使用

### 1. 使用动态库

```cpp
// 包含头文件
#include <nut/nut.h>

// 链接导入库
#pragma comment(lib, "nut.lib")
```

### 2. 使用静态库

```cpp
// 定义静态库宏
#define NUT_STATIC_LIB

// 包含头文件  
#include <nut/nut.h>

// 链接静态库
#pragma comment(lib, "nut_static.lib")
```

### 3. CMake集成

```cmake
# 查找NUT库
find_package(nut REQUIRED)

# 链接到你的目标
target_link_libraries(your_target nut::nut)
```

## 特性

### UTF-8编码支持
所有源文件现在使用UTF-8编码编译，解决了中文字符和特殊字符的显示问题。

### 多处理器编译
自动启用MSVC的多处理器编译 (`/MP`)，显著提升编译速度。

### 警告处理
已配置适当的警告抑制，减少不必要的C4251等DLL接口警告。

### 跨平台兼容
同样的CMake配置在Linux和macOS上也能正常工作。

## 故障排除

### 1. 编码问题
如果遇到编码相关错误，确保：
- 使用Visual Studio 2017或更高版本
- CMake版本3.10或更高

### 2. 运行时库不匹配
确保你的应用程序使用与NUT库相同的运行时库类型：
- 如果使用MT版本的NUT，你的应用也应该使用/MT
- 如果使用MD版本的NUT，你的应用也应该使用/MD

### 3. 找不到DLL
使用动态库时，确保：
- nut.dll在系统PATH中
- 或者将nut.dll复制到你的应用程序目录

## 性能对比

| 配置 | 库大小 | 编译时间 | 运行时性能 | 分发便利性 |
|------|--------|----------|------------|------------|
| MD + SHARED | 小 | 快 | 好 | 需要分发DLL |
| MD + STATIC | 大 | 中 | 好 | 独立可执行文件 |
| MT + SHARED | 小 | 快 | 好 | 需要分发DLL |
| MT + STATIC | 大 | 慢 | 最好 | 完全独立 |

## 总结

新的CMake构建系统为Windows开发者提供了极大的灵活性，支持所有常见的构建配置。无论你需要什么样的运行时库组合，都可以轻松构建和使用NUT库。

