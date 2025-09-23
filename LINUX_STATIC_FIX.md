# Linux 静态库构建问题修复

## 问题描述

在Linux下使用CMake构建NUT库时，无论设置`NUT_LIBRARY_TYPE=STATIC`还是`NUT_LIBRARY_TYPE=SHARED`，都只生成动态库文件（`libnut.so`），无法生成静态库文件（`libnut.a`）。

## 问题原因

在`src/nut/CMakeLists.txt`文件中，第20-25行的逻辑有误：

```cmake
# 根据配置创建库（动态库或静态库）
if(WIN32)
    add_library(nut ${NUT_LIBRARY_TYPE} ${NUT_SOURCES} ${NUT_HEADERS})
else()
    # 非Windows平台默认创建动态库
    add_library(nut SHARED ${NUT_SOURCES} ${NUT_HEADERS})  # ❌ 问题在这里
endif()
```

**问题所在**：在Linux下，无论`NUT_LIBRARY_TYPE`设置为什么值，都强制创建`SHARED`（动态库），完全忽略了用户的配置。

## 修复方案

### 1. 修复库类型创建逻辑

```cmake
# 根据配置创建库（动态库或静态库）
if(WIN32)
    add_library(nut ${NUT_LIBRARY_TYPE} ${NUT_SOURCES} ${NUT_HEADERS})
else()
    # 非Windows平台也支持NUT_LIBRARY_TYPE配置
    if(DEFINED NUT_LIBRARY_TYPE)
        add_library(nut ${NUT_LIBRARY_TYPE} ${NUT_SOURCES} ${NUT_HEADERS})
    else()
        # 默认创建动态库
        add_library(nut SHARED ${NUT_SOURCES} ${NUT_HEADERS})
    endif()
endif()
```

### 2. 统一库类型特定设置

将原来只在Windows下处理的库类型设置扩展到所有平台：

```cmake
# 库类型特定设置
if(NUT_LIBRARY_TYPE STREQUAL "STATIC")
    # 静态库特定设置
    target_compile_definitions(nut PUBLIC NUT_STATIC_LIB)
    if(WIN32)
        set_target_properties(nut PROPERTIES
            OUTPUT_NAME "nut_static"
        )
    else()
        # Linux/macOS静态库设置
        set_target_properties(nut PROPERTIES
            OUTPUT_NAME "nut"
            ARCHIVE_OUTPUT_NAME "nut"
        )
    endif()
    message(STATUS "Building nut as static library")
else()
    # 动态库特定设置
    target_compile_definitions(nut PRIVATE BUILDING_NUT_DLL)
    target_compile_definitions(nut PUBLIC NUT_SHARED_LIB)
    message(STATUS "Building nut as dynamic library")
endif()
```

### 3. 修复符号链接创建逻辑

确保只在动态库时创建符号链接：

```cmake
# 为开发构建创建符号链接（仅Unix系统的动态库）
if(UNIX AND NOT APPLE AND NUT_LIBRARY_TYPE STREQUAL "SHARED")
    add_custom_command(TARGET nut POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        $<TARGET_FILE:nut>
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libnut.so
        COMMENT "Creating symlink for nut library"
    )
endif()
```

### 4. 修复输出信息

让输出信息正确显示实际的库类型：

```cmake
# 输出库信息
message(STATUS "NUT Library Configuration:")
message(STATUS "  Library Type: ${NUT_LIBRARY_TYPE}")  # 显示实际类型
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Sources: ${SOURCE_COUNT} files")
message(STATUS "  Headers: ${HEADER_COUNT} files")
message(STATUS "  Install Path: ${CMAKE_INSTALL_LIBDIR}")
```

## 验证方法

### 1. 使用测试脚本

运行提供的测试脚本：

```bash
chmod +x test-linux-static.sh
./test-linux-static.sh
```

### 2. 使用构建脚本

运行更新后的Linux构建脚本：

```bash
chmod +x build-all-linux.sh
./build-all-linux.sh
```

### 3. 手动验证

```bash
# 构建静态库
cmake -S . -B build-static -DCMAKE_BUILD_TYPE=Release -DNUT_LIBRARY_TYPE=STATIC
cmake --build build-static

# 检查生成的文件
find build-static -name "*.a"  # 应该找到 libnut.a
find build-static -name "*.so" # 应该找不到任何 .so 文件

# 构建动态库
cmake -S . -B build-shared -DCMAKE_BUILD_TYPE=Release -DNUT_LIBRARY_TYPE=SHARED
cmake --build build-shared

# 检查生成的文件
find build-shared -name "*.so"  # 应该找到 libnut.so
find build-shared -name "*.a"   # 应该找不到任何 .a 文件
```

## 预期结果

修复后，Linux下的构建应该产生以下结果：

### 静态库构建 (`NUT_LIBRARY_TYPE=STATIC`)
- ✅ 生成 `libnut.a` 静态库文件
- ✅ 不生成 `libnut.so` 动态库文件
- ✅ 设置 `NUT_STATIC_LIB` 编译定义
- ✅ 输出信息显示 "Building nut as static library"

### 动态库构建 (`NUT_LIBRARY_TYPE=SHARED`)
- ✅ 生成 `libnut.so` 动态库文件
- ✅ 不生成 `libnut.a` 静态库文件
- ✅ 设置 `NUT_SHARED_LIB` 编译定义
- ✅ 输出信息显示 "Building nut as dynamic library"
- ✅ 创建 `libnut.so` 符号链接（仅Linux）

## 兼容性

此修复保持了与现有代码的完全兼容性：
- Windows构建逻辑保持不变
- macOS构建逻辑保持不变
- 现有的构建脚本和配置继续有效
- 只是修复了Linux下被忽略的`NUT_LIBRARY_TYPE`配置
