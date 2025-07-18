# Build Instructions for Blot

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

## Building and Running Apps in `apps/`

The Blot framework is now app-agnostic. All runnable applications are located in the `apps/` directory. Each app in `apps/` is built as a separate executable.

### How to Build All Apps

By default CMake now builds the framework library **and** every sample application found under `apps/`.  This is controlled by the option `BUILD_APPS` (ON by default):

```bash
# turn apps off (library-only build)
cmake -B build -DBUILD_APPS=OFF
# turn apps on explicitly
cmake -B build -DBUILD_APPS=ON
```

With apps enabled you will find a separate executable for each subdirectory—for example:

```
build/Release/
    sample_ui.exe
    sample_blend2d_gradient.exe
    sample_script_engine.exe
```

`BlotApp.exe` was an earlier placeholder and no longer exists.

### How to Run a Specific App

After building, navigate to the output directory and run the desired app, e.g.:

```bash
cd build/Release  # Windows
# or cd build       # Linux/macOS

# UI demo
sample_ui.exe        # Windows
./sample_ui          # Linux/macOS

# Other demos
sample_blend2d_gradient.exe
sample_script_engine.exe
```

## Creating a New App

To create a new app:
1. Copy the `apps/workingTemplate` directory to a new folder under `apps/` (e.g., `apps/MyNewApp`).
2. Rename files and update the CMake target name in `CMakeLists.txt` as needed.
3. Add your app's logic to `main.cpp` and any other files.
4. Add a line to `apps/CMakeLists.txt`:
   ```cmake
   add_subdirectory(MyNewApp)
   ```
5. Rebuild the project. Your new app will be built as its own executable.

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

## Project Structure

```
blot/
├── src/                    # Framework source files
├── apps/                  # All runnable apps
│   ├── BlotApp/            # Example app
│   ├── sample_blend2d_gradient/
│   ├── sample_script_engine/
│   └── workingTemplate/    # Template for new apps
├── CMakeLists.txt          # Build configuration
├── vcpkg.json              # Dependencies
├── build.bat               # Windows build script
├── build.sh                # Linux/macOS build script
└── README.md               # Project documentation
``` 