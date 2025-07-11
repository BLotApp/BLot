# Build Instructions for BLot

## Prerequisites

### 1. Install vcpkg
```bash
# Clone vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg

# Bootstrap vcpkg
./bootstrap-vcpkg.sh  # On Linux/macOS
# or
.\bootstrap-vcpkg.bat  # On Windows

# Set environment variable (add to your shell profile)
export VCPKG_ROOT=/path/to/vcpkg  # Linux/macOS
set VCPKG_ROOT=C:\path\to\vcpkg   # Windows
```

### 2. Install Dependencies
```bash
# Install all dependencies
vcpkg install glfw3 freetype glm glad imgui entt blend2d

# Or install individually
vcpkg install glfw3
vcpkg install freetype
vcpkg install glm
vcpkg install glad
vcpkg install imgui
vcpkg install entt
vcpkg install blend2d
```

### 3. Build the Project

#### Windows
```bash
# Set VCPKG_ROOT environment variable
set VCPKG_ROOT=C:\path\to\vcpkg

# Run the build script
.\build.bat
```

#### Linux/macOS
```bash
# Set VCPKG_ROOT environment variable
export VCPKG_ROOT=/path/to/vcpkg

# Run the build script
./build.sh
```

#### Manual Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Release
make
```

## Troubleshooting

### Common Issues

1. **vcpkg not found**
   - Make sure VCPKG_ROOT environment variable is set
   - Ensure vcpkg is properly installed and bootstrapped

2. **Dependencies not found**
   - Run `vcpkg install` for all required packages
   - Check that the vcpkg.json file is in the project root

3. **CMake configuration errors**
   - Ensure CMake 3.16+ is installed
   - Verify the toolchain file path is correct

4. **Compiler errors**
   - Use a C++17 compatible compiler
   - On Windows, use Visual Studio 2019 or later

### Dependencies List

- **GLFW3**: Window management and input handling
- **FreeType**: Font rendering
- **GLM**: Mathematics library
- **GLAD**: OpenGL loading
- **Dear ImGui**: User interface
- **EnTT**: Entity-Component-System
- **Blend2D**: 2D graphics rendering

### Development Setup

For development, you may also want to install:
- **Visual Studio Code** with C++ extensions
- **CMake Tools** extension for VS Code
- **clang-format** for code formatting

## Running the Application

After successful build:
```bash
cd build/Release  # Windows
# or
cd build          # Linux/macOS

./blot  # Linux/macOS
# or
blot.exe  # Windows
```

## Project Structure

```
blot/
├── src/                    # Source files
│   ├── addons/            # Addon implementations
│   ├── BlotApp.cpp        # Main application
│   ├── Canvas.cpp         # Creative coding canvas
│   ├── Graphics.cpp       # Drawing operations
│   └── ...
├── CMakeLists.txt         # Build configuration
├── vcpkg.json            # Dependencies
├── build.bat             # Windows build script
├── build.sh              # Linux/macOS build script
└── README.md             # Project documentation
``` 