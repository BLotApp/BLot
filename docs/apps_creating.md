# Creating a new Blot app

This guide explains how to start a new application based on **Blot** and pull the addons you need with [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake).

## 1. Copy the template folder

```
cp -r apps/appTemplate apps/myNewApp
```

Rename the target in `apps/myNewApp/CMakeLists.txt` if desired.

## 2. Declare your addons

Each app keeps an `addons.cmake` file alongside its `CMakeLists.txt`.  Use one `CPMAddPackage()` block per addon.

Example (`apps/myNewApp/addons.cmake`):

```cmake
include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

# Local addon during development
CPMAddPackage(
    NAME bxTemplate
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/addons/bxTemplate
)

# Remote addon from GitHub (once the addon lives in its own repo)
CPMAddPackage(
    NAME bxImGui
    GITHUB_REPOSITORY my-org/bxImGui
    GIT_TAG main            # branch, tag, or commit
)
```

## 3. Wire it up in CMakeLists.txt

Your app’s `CMakeLists.txt` just needs to include the manifest **before** declaring the executable and then link against the addon targets:

```cmake
cmake_minimum_required(VERSION 3.10)
project(myNewApp)

file(GLOB APP_SOURCES *.cpp *.h)

# Pull in CPM + addons specified in addons.cmake
include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/addons.cmake)

add_executable(myNewApp ${APP_SOURCES})

target_link_libraries(myNewApp PRIVATE blot bxTemplate bxImGui)
```

## 4. Build

From your build directory:

```
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target myNewApp -j
```

CPM downloads (or reuses cached) addons automatically at configure time.

## 5. Updating an addon

Simply change the `GIT_TAG` (or switch back to a local `SOURCE_DIR`) in `addons.cmake` and re-configure.

## 6. Advantages of the CPM approach

* **Per-app isolation** – each application lists only the addons it needs.
* **Zero submodule overhead** – repository stays small and clean.
* **Reproducible builds** – you lock the exact commit with `GIT_TAG`.
* **Fast iteration** – CPM caches each external project in your build tree.

For more information see the official CPM.cmake documentation: <https://github.com/cpm-cmake/CPM.cmake> 
