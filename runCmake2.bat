@echo off
if not exist build (mkdir build)
cd build
D:\cmake-3.31.4-windows-x86_64\bin\cmake ..
cd ..
echo You can now open Application.sln in the build directory