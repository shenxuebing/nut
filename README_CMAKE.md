# NUT Library CMake 构建系统

这个文档描述了如何使用CMake构建系统来编译NUT库，支持Windows、Linux、macOS等多个平台。

## 目录结构

```
nut/
├── CMakeLists.txt              # 主CMake配置文件
├── build.py                    # Python构建脚本（跨平台）
├── build.bat                   # Windows批处理构建脚本
├── build.sh                    # Unix shell构建脚本
├── cmake/
│   └── nutConfig.cmake.in      # CMake包配置模板
├── src/
│   ├── nut/
│   │   ├── CMakeLists.txt      # nut库CMake配置
│   │   └── ...                 # 源代码文件
│   └── test_nut/
│       ├── CMakeLists.txt      # 测试程序CMake配置
│       └── ...                 # 测试源代码文件
└── proj/                       # 原有的构建配置（保留用于参考）
```

## 系统要求

### 通用要求
- CMake 3.10 或更高版本
- C++11 兼容的编译器
- C11 兼容的C编译器

### Windows
- Visual Studio 2017 或更高版本（推荐 VS 2019/2022）
- 或者 MinGW-w64
- 或者 Clang

### Linux
- GCC 4.9 或更高版本
- 或者 Clang 3.5 或更高版本
- pthread 库
- dl 库（动态链接）
- atomic 库（某些平台）

### macOS
- Xcode 9 或更高版本
- 或者 Homebrew GCC/Clang

## 快速开始

### 使用构建脚本（推荐）

#### Windows
```batch
# 使用默认设置构建（MD + SHARED）
build.bat

# 构建MT静态库
build.bat --runtime-lib MT --lib-type STATIC

# 构建所有组合（MT/MD + SHARED/STATIC）
build.bat --build-all

# 构建Debug版本
build.bat --build-type Debug

# 使用Visual Studio 2019
build.bat --generator "Visual Studio 16 2019" all

# 显示帮助
build.bat --help
```

#### Python脚本（跨平台）
```bash
# 使用默认设置构建
python build.py

# Windows: 构建MT静态库
python build.py --runtime-lib MT --lib-type STATIC

# Windows: 构建所有组合
python build.py --build-all

# 构建Debug版本并运行测试
python build.py --build-type Debug all

# 使用Ninja构建器
python build.py --generator Ninja

# 显示帮助
python build.py --help
```

#### Unix系统（Linux/macOS）
```bash
# 使用默认设置构建
./build.sh

# 构建Debug版本
./build.sh --build-type Debug

# 使用8个并行任务
./build.sh --jobs 8

# 在Linux上使用Valgrind检查内存
./build.sh valgrind
```

### 手动使用CMake

#### 基本构建
```bash
# 创建构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 构建
cmake --build .

# 运行测试
./bin/test_nut    # Linux/macOS
.\bin\test_nut.exe  # Windows
```

#### 指定构建类型
```bash
# Debug版本
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release版本
cmake -DCMAKE_BUILD_TYPE=Release ..

# 带调试信息的Release版本
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
```

#### 指定生成器
```bash
# Visual Studio
cmake -G "Visual Studio 16 2019" ..

# Ninja
cmake -G "Ninja" ..

# Unix Makefiles
cmake -G "Unix Makefiles" ..
```

#### 多配置生成器（如Visual Studio）
```bash
# 配置
cmake -G "Visual Studio 16 2019" ..

# 构建Debug版本
cmake --build . --config Debug

# 构建Release版本
cmake --build . --config Release
```

## 构建选项

### CMake选项
- `CMAKE_BUILD_TYPE`: 构建类型（Debug/Release/RelWithDebInfo/MinSizeRel）
- `CMAKE_INSTALL_PREFIX`: 安装前缀路径
- `INSTALL_TESTS`: 是否安装测试程序（默认OFF）

#### Windows特定选项
- `NUT_RUNTIME_LIBRARY`: 运行时库类型（MT/MD，默认MD）
- `NUT_LIBRARY_TYPE`: 库类型（SHARED/STATIC，默认SHARED）

### 平台特定选项

#### Windows (MSVC)
```bash
# 构建MD动态库（默认）
cmake -DCMAKE_BUILD_TYPE=Release ..

# 构建MT静态库
cmake -DCMAKE_BUILD_TYPE=Release -DNUT_RUNTIME_LIBRARY=MT -DNUT_LIBRARY_TYPE=STATIC ..

# 构建MD静态库
cmake -DCMAKE_BUILD_TYPE=Release -DNUT_LIBRARY_TYPE=STATIC ..

# 构建MT动态库
cmake -DCMAKE_BUILD_TYPE=Release -DNUT_RUNTIME_LIBRARY=MT ..
```

#### 输出目录结构（Windows）
```
build/
├── MD/
│   ├── shared/        # MD动态库
│   │   ├── bin/       # DLL和可执行文件
│   │   └── lib/       # 导入库
│   └── static/        # MD静态库
│       └── lib/       # 静态库文件
└── MT/
    ├── shared/        # MT动态库
    │   ├── bin/       # DLL和可执行文件
    │   └── lib/       # 导入库
    └── static/        # MT静态库
        └── lib/       # 静态库文件
```

#### Linux
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
```

#### macOS
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" ..
```

## 安装

### 安装到系统
```bash
# 配置时指定安装前缀
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..

# 构建
cmake --build .

# 安装（可能需要管理员权限）
cmake --install .
```

### Windows安装示例
```batch
# 配置
cmake -DCMAKE_INSTALL_PREFIX=C:\Program Files\nut ..

# 构建
cmake --build . --config Release

# 安装（以管理员身份运行）
cmake --install . --config Release
```

## 在其他项目中使用NUT库

### 使用find_package
```cmake
# 在你的CMakeLists.txt中
find_package(nut REQUIRED)

# 链接到你的目标
target_link_libraries(your_target nut::nut)
```

### 使用pkg-config（Unix系统）
```bash
# 编译
g++ -std=c++11 $(pkg-config --cflags nut) your_code.cpp $(pkg-config --libs nut)
```

## 测试

### 运行单元测试
```bash
# 直接运行
./build/bin/test_nut

# 或使用构建脚本
build.bat test           # Windows
./build.sh test          # Unix
python build.py test     # 跨平台
```

### 内存检查（Linux）
```bash
# 使用Valgrind
./build.sh valgrind

# 或手动运行
valgrind --leak-check=full ./build/bin/test_nut
```

## 故障排除

### 常见问题

#### 1. CMake版本过低
```
错误: CMake 3.10 or higher is required
解决: 升级CMake到3.10或更高版本
```

#### 2. 编译器不支持C++11
```
错误: This compiler does not support C++11
解决: 使用支持C++11的编译器，如GCC 4.9+, Clang 3.5+, MSVC 2017+
```

#### 3. 找不到pthread（Linux）
```
错误: Could not find Threads
解决: sudo apt-get install libpthread-stubs0-dev  # Ubuntu/Debian
       yum install glibc-devel                     # CentOS/RHEL
```

#### 4. 链接错误（Windows）
```
错误: unresolved external symbol
解决: 确保使用相同的运行时库设置（/MD, /MT等）
```

### 调试构建问题

#### 启用详细输出
```bash
cmake --build . --verbose

# 或
make VERBOSE=1  # 使用Make时
```

#### 查看CMake变量
```bash
cmake -LAH ..
```

#### 查看编译器信息
```bash
cmake --system-information
```

## 与原有构建系统的比较

| 特性 | Makefile | CMake |
|------|----------|-------|
| 跨平台支持 | 有限 | 优秀 |
| IDE集成 | 无 | 优秀 |
| 依赖管理 | 手动 | 自动 |
| 包管理 | 无 | 支持 |
| 维护性 | 较难 | 较易 |
| 学习曲线 | 陡峭 | 适中 |

## 贡献

如果你发现构建系统的问题或有改进建议，请：

1. 提交Issue描述问题
2. 如果可能，提供修复的Pull Request
3. 确保在多个平台上测试你的修改

## 许可证

构建系统配置文件遵循与NUT库相同的许可证。
