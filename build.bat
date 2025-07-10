@echo off
setlocal enabledelayedexpansion

:: Default to Release build
set BUILD_TYPE=Release

:: Check for --debug flag
if "%1"=="--debug" (
    set BUILD_TYPE=Debug
)

echo ========================================
echo Building Blot - Creative Coding Application
echo ========================================

:: Auto-detect and setup vcpkg
if "%VCPKG_ROOT%"=="" (
    echo VCPKG_ROOT not set, attempting to auto-detect vcpkg...
    
    :: Check if vcpkg exists in current directory
    if exist "%~dp0vcpkg\vcpkg.exe" (
        set VCPKG_ROOT=%~dp0vcpkg
        echo Found vcpkg in current directory
        goto continue
    ) else if exist "%USERPROFILE%\vcpkg\vcpkg.exe" (
        set VCPKG_ROOT=%USERPROFILE%\vcpkg
        echo Found vcpkg at %USERPROFILE%\vcpkg
        goto continue
    ) else if exist "C:\vcpkg\vcpkg.exe" (
        set VCPKG_ROOT=C:\vcpkg
        echo Found vcpkg at C:\vcpkg
        goto continue
    ) else (
        echo vcpkg not found in common locations.
        echo.
        echo Would you like to install vcpkg automatically? (Y/N)
        set /p install_vcpkg=
        if /i "!install_vcpkg!"=="Y" (
            echo Installing vcpkg...
            
            :: Check if Git is available
            git --version >nul 2>&1
            if errorlevel 1 (
                echo Git not found. Please install Git first.
                echo You can download Git from: https://git-scm.com/downloads
                pause
                exit /b 1
            )
            
            cd %USERPROFILE%
            git clone https://github.com/Microsoft/vcpkg.git
            cd vcpkg
            call bootstrap-vcpkg.bat
            set VCPKG_ROOT=%USERPROFILE%\vcpkg
            echo vcpkg installed at %VCPKG_ROOT%
            cd /d "%~dp0"
            echo.
            echo vcpkg installation completed successfully!
        ) else (
            echo Please install vcpkg manually or set VCPKG_ROOT environment variable.
            echo See BUILD_INSTRUCTIONS.md for detailed instructions.
            pause
            exit /b 1
        )
    )
)
:continue
echo Continuing after vcpkg detection...
:: Check if CMake is available
cmake --version >nul 2>&1
echo Checked CMake version.
if errorlevel 1 (
    echo CMake not found. Please install CMake and add it to your PATH.
    echo You can download CMake from: https://cmake.org/download/
    pause
    exit /b 1
)

:: Check if Git is available (needed for vcpkg)
git --version >nul 2>&1
echo Checked Git version.
if errorlevel 1 (
    echo Git not found. Please install Git and add it to your PATH.
    echo You can download Git from: https://git-scm.com/downloads
    pause
    exit /b 1
)

:: Check if Visual Studio is available
where cl >nul 2>&1
echo Checked for Visual Studio compiler.
if errorlevel 1 goto no_vs

goto vs_found

:no_vs
echo Visual Studio compiler not found in PATH.
echo Attempting to launch a Visual Studio Developer Command Prompt...
set "VS_PROMPT=%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
echo VS_PROMPT is [%VS_PROMPT%]
if exist "%VS_PROMPT%" (
    start "" cmd /k "%VS_PROMPT%"
    echo.
    echo A new Developer Command Prompt has been opened.
    echo Please re-run build.bat in that window to continue the build.
    pause
    exit /b 1
)
echo Could not find the Developer Command Prompt script automatically.
echo Please open it manually from the Start Menu (search for "x64 Native Tools Command Prompt for VS 2022").
pause
exit /b 1

:vs_found

:: Ensure submodules are initialized and updated
if exist ".gitmodules" (
    echo Initializing/updating git submodules...
    git submodule update --init --recursive
    if errorlevel 1 (
        echo Failed to update submodules. Please check your git configuration.
        pause
        exit /b 1
    )
)

:: Create build directory
if not exist "build" mkdir build
cd build

echo Installing dependencies with vcpkg manifest...
"%VCPKG_ROOT%\vcpkg.exe" install
echo Dependencies installed (manifest mode).

echo Configuring with CMake...
cmake .. -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
echo Ran CMake configure.
if errorlevel 1 (
    echo CMake configuration failed.
    echo.
    echo This might be due to missing dependencies. Trying to install them...
    "%VCPKG_ROOT%\vcpkg.exe" install
    echo.
    echo Retrying CMake configuration...
    cmake .. -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
    if errorlevel 1 (
        echo CMake configuration still failed. Please check the error messages above.
        pause
        exit /b 1
    )
)

echo Building Blot...
cmake --build . --config %BUILD_TYPE% --parallel
echo Ran build command.
if errorlevel 1 (
    echo Build failed.
    pause
    exit /b 1
)

if exist "%BUILD_TYPE%\blot.exe" (
    echo.
    echo ========================================
    echo Build successful!
    echo ========================================
    echo.
    echo Executable: build\%BUILD_TYPE%\blot.exe
    echo.
    echo To run Blot:
    echo   cd build\%BUILD_TYPE%
    echo   blot.exe
    echo.
) else (
    echo.
    echo Build completed but executable not found.
    echo Check the build directory for any errors.
    echo.
)

:: Always keep the window open at the end so the user can see the output
pause
cmd /k 