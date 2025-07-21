# Creating a Blot Addon

This guide shows how to structure a standalone addon and how to declare 
other addons it depends on so that users only need a **single**
`CPMAddPackage()` line in their application.

---
## 1. Directory layout

```
addons/
  bxMyAddon/
    CMakeLists.txt      <-- build script (see below)
    bxMyAddon.cpp
    bxMyAddon.h
    README.md           <-- optional
```

Your addon can live in its own repository or inside the main Blot repo
while under development.

---
## 2. Minimal `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.10)

# -------------------------------------------------------------
# Resolve addon-level dependencies via CPM
# -------------------------------------------------------------
include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

# Example: this addon needs bxImGui
CPMAddPackage(
    NAME bxImGui
    GITHUB_REPOSITORY BLotApp/bxImGui
    GIT_TAG main
)

add_library(bxMyAddon STATIC bxMyAddon.cpp bxMyAddon.h)

target_link_libraries(bxMyAddon PUBLIC blot bxImGui)

target_include_directories(bxMyAddon PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)

message(STATUS "Configured addon: bxMyAddon (auto-fetches bxImGui)")
```

Key points
1. **include CPM.cmake** – reuse the same package manager the apps use.
2. **CPMAddPackage for every dependency** – this downloads / configures
   the required addon *before* your target is evaluated.
3. **Link against blot + dependencies** – their include directories are
   propagated automatically.

With this pattern, an application only needs:

```cmake
CPMAddPackage(
    NAME bxMyAddon
    GITHUB_REPOSITORY BLotApp/bxMyAddon
    GIT_TAG main
)
```

and CMake will fetch bxMyAddon *and* any dependencies it declared.

---
## 3. Optional examples

If your addon ships example apps, place them in an `examples/` folder and
make their build optional:

```cmake
option(BUILD_BXMYADDON_EXAMPLES "Build bxMyAddon examples" OFF)
if(BUILD_BXMYADDON_EXAMPLES AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/examples")
    add_subdirectory(examples)
endif()
```

Users enable them with:
```
cmake .. -DBUILD_BXMYADDON_EXAMPLES=ON
```

---
## 4. Publishing & versioning

• Tag releases in your addon repo so applications can lock to a specific
  version (`GIT_TAG v1.2.0`).
• Keep your `main` branch stable so `GIT_TAG main` remains usable.

Following this template ensures addons remain self-contained and their
transitive dependencies are resolved automatically through CPM.cmake. 
