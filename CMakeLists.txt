cmake_minimum_required(VERSION 3.19)

# vcpkg toolchain logic removed – project now builds entirely from vendored
# submodules and system libraries.

project(Blot VERSION 1.0.0 LANGUAGES C CXX)

# ----------------------------------------------------
# CPM.cmake – lightweight CMake package manager
# This lets each application fetch only the addons it needs
# ----------------------------------------------------
set(CPM_DOWNLOAD_VERSION "0.38.2")
set(CPM_PATH "${CMAKE_SOURCE_DIR}/cmake/CPM.cmake")
if(NOT EXISTS "${CPM_PATH}")
    message(STATUS "Downloading CPM.cmake (v${CPM_DOWNLOAD_VERSION})")
    file(DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
        "${CPM_PATH}"
        TLS_VERIFY ON)
endif()
include(${CPM_PATH})

option(BUILD_ADDON_EXAMPLES "Build all addon examples" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required system OpenGL (minimal external dependency)
find_package(OpenGL REQUIRED)

# Ensure our submodule GLFW is used rather than a system one
set(GLFW3_FOUND OFF CACHE BOOL "" FORCE)
set(GLFW_FOUND OFF CACHE BOOL "" FORCE)
set(GLFW3_USE_STATIC_LIBS ON CACHE BOOL "" FORCE)

# Add GLM and EnTT from submodules
include_directories(${CMAKE_SOURCE_DIR}/third_party/glm)
include_directories(${CMAKE_SOURCE_DIR}/third_party/entt/single_include)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Automatically collect all source and header files in src/
file(GLOB_RECURSE BLOT_SOURCES
    src/*.cpp
    src/*.c
    src/*.h
    src/*.hpp
)
# Exclude third_party sources if needed
list(FILTER BLOT_SOURCES EXCLUDE REGEX "third_party/")

# ImGui sources are now handled by the bxImGui addon
set(IMGUI_SOURCES)
set(IMGUI_BACKEND_SOURCES)
set(IMGUI_COLOR_TEXT_EDIT_SOURCES)

# ----------------- Addon hybrid build -------------------

# No longer building in-tree addons here – each application fetches and
# builds the addons it needs via CPM.cmake. If you still want to build an
# addon globally, add it explicitly with add_subdirectory() above this
# comment.


# Organize source files into folders for Visual Studio
source_group("Canvas" FILES src/framework/canvas/Canvas.cpp src/framework/canvas/MCanvas.cpp src/framework/canvas/MCanvas.h)
source_group("Rendering" FILES src/rendering/Graphics.cpp)

# GLOB all Addons sources and headers
file(GLOB ADDONS_SOURCES src/addons/*.cpp src/addons/*.h)
source_group("Addons" FILES ${ADDONS_SOURCES})

# GLOB all Resources sources and headers
file(GLOB RESOURCES_SOURCES src/resources/*.cpp src/resources/*.h)
source_group("Resources" FILES ${RESOURCES_SOURCES})

# GLOB all ECS sources and headers
file(GLOB ECS_SOURCES src/ecs/*.cpp src/ecs/*.h src/ecs/systems/*.cpp src/ecs/systems/*.h)
source_group("ECS" FILES ${ECS_SOURCES})

# GLOB all Addon sources and headers
file(GLOB ADDON_SOURCES src/core/addon/*.cpp src/core/addon/*.h)
source_group("Addon" FILES ${ADDON_SOURCES})

# UI window sources moved to bxImGui addon

# Node sources moved to bxNodeEditor addon

# GLOB all Scripting sources and headers
file(GLOB SCRIPTING_SOURCES src/scripting/*.cpp src/scripting/*.h)
source_group("Scripting" FILES ${SCRIPTING_SOURCES})

# Use the globbed files in the source group and target sources
source_group("UI" FILES src/ui/ImGuiRenderer.cpp src/ui/CoordinateSystem.cpp src/ui/Mui.cpp)
source_group("ECS" FILES src/ecs/MEcs.cpp src/ecs/systems/CanvasSystems.cpp src/ecs/systems/SEvent.cpp)
source_group("Scripting" FILES src/scripting/ScriptEngine.cpp) 

# Propagate third-party include directories needed by external consumers (apps)
target_include_directories(blot PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/third_party/glfw/include
    ${CMAKE_SOURCE_DIR}/third_party/spdlog/include
)

add_subdirectory(third_party/glfw) 

target_include_directories(blot PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/core/addon
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/rendering
    ${CMAKE_SOURCE_DIR}/src/ecs
    ${CMAKE_SOURCE_DIR}/src/core/canvas
    # ...add others as needed
) 

# TODO: This is a hack to ensure the core has ImGui headers available (used by MEcs, etc.)  
# PLEASE FIX THIS simply search the headers and remove implement properly through registers
# Ensure core has ImGui headers available (used by MEcs, etc.)
# set(IMGUI_LOCAL_DIR "${CMAKE_SOURCE_DIR}/addons/bxImGui/imgui")
# if (NOT EXISTS "${IMGUI_LOCAL_DIR}/imgui.h")
#     CPMAddPackage(
#         NAME imgui_core
#         GITHUB_REPOSITORY ocornut/imgui
#         GIT_TAG docking
#     )
#     set(IMGUI_INCLUDE_DIR ${imgui_core_SOURCE_DIR})
# else()
#     set(IMGUI_INCLUDE_DIR ${IMGUI_LOCAL_DIR})
# endif() 

# Add the ImGui include dir so blot's sources can find imgui.h
# target_include_directories(blot PUBLIC ${IMGUI_INCLUDE_DIR})

# Build the blot library first to ensure it's not affected by addon macros
add_library(blot STATIC ${BLOT_SOURCES})
# ImGui sources removed from core target
target_sources(blot PRIVATE ${APP_PATHS_SOURCES})

# Use local glad (not vcpkg or submodule target)
target_include_directories(blot PUBLIC ${CMAKE_SOURCE_DIR}/third_party/glad/include)

if(WIN32)
    target_sources(blot PRIVATE third_party/glad/src/gl.c)
elseif(UNIX AND NOT APPLE)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
        target_sources(blot PRIVATE third_party/glad/src/gles2.c)
    else()
        target_sources(blot PRIVATE third_party/glad/src/gl.c)
    endif()
endif()

find_package(OpenGL REQUIRED)

target_link_libraries(blot
    PRIVATE
        OpenGL::GL
        glfw
)

# Add include directories for submodules (ImGui, etc.)
target_include_directories(blot PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/rendering
    ${CMAKE_SOURCE_DIR}/src/ecs
    ${CMAKE_SOURCE_DIR}/third_party/spdlog/include
    ${CMAKE_SOURCE_DIR}/third_party/glfw/include
    ${CMAKE_SOURCE_DIR}/third_party/glad/include
    ${CMAKE_SOURCE_DIR}/third_party/json/single_include
)

# Compiler flags
if(MSVC)
    target_compile_options(blot PRIVATE /W4)
else()
    target_compile_options(blot PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Copy assets
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# Build blot library completely isolated from addons
add_custom_target(blot_library_built ALL DEPENDS blot)

# Only include addons after blot library is fully built
# This ensures blot library is compiled without any ASMJIT macro pollution

include(cmake/BlotAddons.cmake)

# helper macro
function(add_blot_application APP_NAME)
    set(APP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/apps/${APP_NAME}")
    set(APP_MANIFEST "${APP_DIR}/app.json")

    # Read the manifest to get the project name
    if(EXISTS ${APP_MANIFEST})
        file(READ "${APP_MANIFEST}" _manifest)
        string(JSON _project_name GET "${_manifest}" name)
        
        # Sanitize project name for CMake (replace spaces and special chars with underscores)
        string(REGEX REPLACE "[^a-zA-Z0-9_]" "_" _sanitized_name "${_project_name}")
        string(REGEX REPLACE "_+" "_" _sanitized_name "${}")
        string(REGEX REPLACE "^_|_$" "" _sanitized_name "${_sanitized_name}")
        
        # Use sanitized name for CMake project, fallback to APP_NAME if empty
        if(_sanitized_name STREQUAL "")
            set(_sanitized_name ${APP_NAME})
        endif()
        
        message(STATUS "[BlotAddons] Using project name: '${_project_name}' -> '${_sanitized_name}'")
    else()
        set(_sanitized_name ${APP_NAME})
        message(STATUS "[BlotAddons] No app.json found, using APP_NAME: ${_sanitized_name}")
    endif()

    # Resolve addon dependencies declared in the manifest (variable -> APP_ADDON_LIBS)
    blot_load_addons(${APP_MANIFEST} APP_ADDON_LIBS)

    # Collect all sources inside the application directory (headers are optional)
    file(GLOB APP_SRC
        "${APP_DIR}/*.cpp"
        "${APP_DIR}/*.c"
        "${APP_DIR}/*.h"
        "${APP_DIR}/*.hpp"
    )

    add_executable(${_sanitized_name} ${APP_SRC})
    # Link addons before blot to ensure they're in global namespace
    target_link_libraries(${_sanitized_name} PRIVATE ${APP_ADDON_LIBS} blot)
    target_include_directories(${_sanitized_name} PRIVATE ${CMAKE_SOURCE_DIR})
endfunction()

# Build blot library first, then build applications with addons
# This ensures blot library is compiled without any ASMJIT macro pollution

# Automatically discover and build all apps
file(GLOB APP_DIRS LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/apps/*")
foreach(APP_DIR ${APP_DIRS})
    if(IS_DIRECTORY ${APP_DIR})
        get_filename_component(APP_NAME ${APP_DIR} NAME)
        add_blot_application(${APP_NAME})
    endif()
endforeach() 

