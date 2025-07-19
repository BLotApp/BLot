# Build Instructions for Blot

## Prerequisites

* CMake ≥ 3.16
* A C++17-capable compiler (MSVC ≥ 17, Clang ≥ 10, GCC ≥ 9)
* Git (for cloning submodules)

All third-party code is vendored as git submodules, please do not use an external package manager.

---

## One-shot build (Windows)

```powershell
REM Clone and pull submodules
git clone https://github.com/<you>/blot.git
cd blot
git submodule update --init --recursive

REM Build the framework and all sample apps
quick_build.bat Release sample_app   # or sample_menubar etc.
```

## One-shot build (Linux / macOS)

```bash
# Clone and pull submodules
git clone https://github.com/<you>/blot.git
cd blot
git submodule update --init --recursive

# Configure + build
./build.sh Release sample_app
```

Both helper scripts run `cmake -S . -B build` on first execution and re-use the generated build folder thereafter.

---

## Manual CMake invocation

```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --target sample_app
```

The top-level `apps/CMakeLists.txt` adds every subdirectory under `apps/` as its own executable target, so they will all be built unless you pass `--target <name>`.

---

## Updating submodules

Any time you pull new commits, make sure to also update submodules:

```bash
git pull
git submodule update --init --recursive
``` 