@echo off
REM Simple build script for Blot – vcpkg no longer required

setlocal

:: Build type (Release by default, pass --debug for Debug)
set BUILD_TYPE=Release
if "%1"=="--debug" set BUILD_TYPE=Debug

:: Ensure submodules are present
if exist .gitmodules (
    echo Updating submodules...
    git submodule update --init --recursive
)

:: Create/refresh build directory
if not exist build mkdir build

:: Configure (runs only first time or when CMakeLists change)
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=
if errorlevel 1 goto :cmake_fail

:: Build library + all apps
cmake --build build --config %BUILD_TYPE%
if errorlevel 1 goto :build_fail

:: Success message
echo.
echo ========================================
echo Build succeeded 
echo Output executables are in build\%BUILD_TYPE%\
echo ========================================
echo.
pause
endlocal & cmd /k

:cmake_fail
echo CMake configuration failed.
pause
endlocal & cmd /k

:build_fail
echo Build failed.
pause
endlocal & cmd /k 