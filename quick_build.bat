@echo off
echo Quick build for Blot...

if not exist "build" (
    echo Build directory not found. Please run the full build script first.
    pause
    exit /b 1
)

cd build

echo Building with CMake...
cmake --build . --config Debug

if %ERRORLEVEL% EQU 0 (
    echo Build successful!
    echo Running Blot...
    cd ..
    build\Debug\blot.exe
) else (
    echo Build failed!
    pause
) 