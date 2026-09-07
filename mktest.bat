@echo off
rem 重新 configure(GLOB 收集新增测试文件) -> 构建 test_nut -> 拷贝依赖 -> 运行
rem 统一使用 VS2026 工具链(与 build-utf8fix 的 CMake 缓存一致，避免 cl/LIB 版本错配)
call "D:\Visual Studio 2026\VC\Auxiliary\Build\vcvarsall.bat" x64
cd /d E:\cpputil\nut-master\nut-master
cmake -S . -B build-utf8fix
if errorlevel 1 exit /b 1
cmake --build build-utf8fix --target test_nut
if errorlevel 1 exit /b 1
copy /y build-utf8fix\bin\nut.dll build-utf8fix\MD\bin\ >nul
build-utf8fix\MD\bin\test_nut.exe %*
