#!/bin/bash

echo "========================================"
echo "Building Blot - Creative Coding Application"
echo "========================================"

# Auto-detect and setup vcpkg
if [ -z "$VCPKG_ROOT" ]; then
    echo "VCPKG_ROOT not set, attempting to auto-detect vcpkg..."
    
    # Check common vcpkg locations
    if [ -d "/usr/local/vcpkg" ]; then
        export VCPKG_ROOT="/usr/local/vcpkg"
        echo "Found vcpkg at /usr/local/vcpkg"
    elif [ -d "$HOME/vcpkg" ]; then
        export VCPKG_ROOT="$HOME/vcpkg"
        echo "Found vcpkg at $HOME/vcpkg"
    elif [ -d "$HOME/Documents/vcpkg" ]; then
        export VCPKG_ROOT="$HOME/Documents/vcpkg"
        echo "Found vcpkg at $HOME/Documents/vcpkg"
    else
        echo "vcpkg not found in common locations."
        echo ""
        read -p "Would you like to install vcpkg automatically? (y/N): " install_vcpkg
        if [[ $install_vcpkg =~ ^[Yy]$ ]]; then
            echo "Installing vcpkg..."
            cd $HOME
            git clone https://github.com/Microsoft/vcpkg.git
            cd vcpkg
            ./bootstrap-vcpkg.sh
            export VCPKG_ROOT="$HOME/vcpkg"
            echo "vcpkg installed at $VCPKG_ROOT"
            cd - > /dev/null
        else
            echo "Please install vcpkg manually or set VCPKG_ROOT environment variable."
            echo "See BUILD_INSTRUCTIONS.md for detailed instructions."
            exit 1
        fi
    fi
fi

# Check if CMake is available
if ! command -v cmake &> /dev/null; then
    echo "CMake not found. Please install CMake and add it to your PATH."
    echo "You can download CMake from: https://cmake.org/download/"
    exit 1
fi

# Check if Git is available (needed for vcpkg)
if ! command -v git &> /dev/null; then
    echo "Git not found. Please install Git and add it to your PATH."
    echo "You can download Git from: https://git-scm.com/downloads"
    exit 1
fi

# Ensure submodules are initialized and updated
if [ -f ".gitmodules" ]; then
    echo "Initializing/updating git submodules..."
    git submodule update --init --recursive
    if [ $? -ne 0 ]; then
        echo "Failed to update submodules. Please check your git configuration."
        exit 1
    fi
fi

# Create build directory
if [ ! -d "build" ]; then
    mkdir build
fi
cd build

# Install dependencies if needed
echo ""
echo "Checking dependencies..."
if [ ! -d "$VCPKG_ROOT/installed/x64-linux/include/glfw" ] && [ ! -d "$VCPKG_ROOT/installed/x64-osx/include/glfw" ]; then
    echo "Installing dependencies..."
    "$VCPKG_ROOT/vcpkg" install glfw3:x64-linux
    "$VCPKG_ROOT/vcpkg" install freetype:x64-linux
    "$VCPKG_ROOT/vcpkg" install glm:x64-linux
    "$VCPKG_ROOT/vcpkg" install glad:x64-linux
    "$VCPKG_ROOT/vcpkg" install imgui:x64-linux
    "$VCPKG_ROOT/vcpkg" install entt:x64-linux
    "$VCPKG_ROOT/vcpkg" install blend2d:x64-linux
    echo "Dependencies installed successfully."
else
    echo "Dependencies already installed."
fi

# Configure with CMake
echo ""
echo "Configuring with CMake..."
cmake .. -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" -DCMAKE_BUILD_TYPE=Release
if [ $? -ne 0 ]; then
    echo "CMake configuration failed."
    echo ""
    echo "This might be due to missing dependencies. Trying to install them..."
    "$VCPKG_ROOT/vcpkg" install glfw3:x64-linux freetype:x64-linux glm:x64-linux glad:x64-linux imgui:x64-linux entt:x64-linux blend2d:x64-linux
    echo ""
    echo "Retrying CMake configuration..."
    cmake .. -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" -DCMAKE_BUILD_TYPE=Release
    if [ $? -ne 0 ]; then
        echo "CMake configuration still failed. Please check the error messages above."
        exit 1
    fi
fi

# Build the project
echo ""
echo "Building Blot..."
make -j$(nproc)
if [ $? -ne 0 ]; then
    echo "Build failed."
    exit 1
fi

# Check if executable was created
if [ -f "blot" ]; then
    echo ""
    echo "========================================"
    echo "Build successful!"
    echo "========================================"
    echo ""
    echo "Executable: build/blot"
    echo ""
    echo "To run Blot:"
    echo "  cd build"
    echo "  ./blot"
    echo ""
else
    echo ""
    echo "Build completed but executable not found."
    echo "Check the build directory for any errors."
    echo ""
fi 