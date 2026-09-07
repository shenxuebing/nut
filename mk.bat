@echo off
rem usage: mk.bat <target>
rem 统一使用 VS2026 工具链(与 build-utf8fix 的 CMake 缓存一致，避免 cl/LIB 版本错配)
call "D:\Visual Studio 2026\VC\Auxiliary\Build\vcvarsall.bat" x64
cd /d E:\cpputil\nut-master\nut-master
cmake --build build-utf8fix --target %1
