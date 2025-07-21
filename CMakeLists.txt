cmake_minimum_required(VERSION 3.16)

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
include_directories(${CMAKE_SOURCE_DIR}/addons/bxImGui/imgui)
include_directories(${CMAKE_SOURCE_DIR}/addons/bxImGui/imgui/backends)
include_directories(${CMAKE_SOURCE_DIR}/third_party/imgui_markdown)
include_directories(${CMAKE_SOURCE_DIR}/third_party/imgui-filebrowser)
include_directories(${CMAKE_SOURCE_DIR}/third_party/ImGuiColorTextEdit)

# Add PortableFileDialogs include path
include_directories(third_party/portable-file-dialogs)

# Add ImPlot and ImPlot3D include directories
include_directories(${CMAKE_SOURCE_DIR}/third_party/implot)
include_directories(${CMAKE_SOURCE_DIR}/third_party/implot3d)

# Automatically collect all source and header files in src/
file(GLOB_RECURSE BLOT_SOURCES
    src/*.cpp
    src/*.c
    src/*.h
    src/*.hpp
)
# Exclude third_party sources if needed
list(FILTER BLOT_SOURCES EXCLUDE REGEX "third_party/")

# GLOB ImGui core sources
file(GLOB IMGUI_SOURCES
    addons/bxImGui/imgui/imgui.cpp
    addons/bxImGui/imgui/imgui_draw.cpp
    addons/bxImGui/imgui/imgui_tables.cpp
    addons/bxImGui/imgui/imgui_widgets.cpp
    addons/bxImGui/imgui/imgui_demo.cpp
)
# GLOB ImGui backends (only the ones you use)
file(GLOB IMGUI_BACKEND_SOURCES
    addons/bxImGui/imgui/backends/imgui_impl_glfw.cpp
    addons/bxImGui/imgui/backends/imgui_impl_opengl3.cpp
)
# GLOB ImGuiColorTextEdit
file(GLOB IMGUI_COLOR_TEXT_EDIT_SOURCES
    third_party/ImGuiColorTextEdit/TextEditor.cpp
)
# imgui-node-editor is now provided by the bxNodeEditor addon.

# ----------------- Addon hybrid build -------------------

# No longer building in-tree addons here – each application fetches and
# builds the addons it needs via CPM.cmake. If you still want to build an
# addon globally, add it explicitly with add_subdirectory() above this
# comment.


add_library(blot STATIC ${BLOT_SOURCES})
target_sources(blot PRIVATE
    ${IMGUI_SOURCES}
    ${IMGUI_BACKEND_SOURCES}
    ${IMGUI_COLOR_TEXT_EDIT_SOURCES}
    ${APP_PATHS_SOURCES}
)

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

# -----------------------------------------------------------------------------
# Optional sample / demo applications
option(BUILD_APPS "Build sample applications in /apps" ON)
if(BUILD_APPS)
    add_subdirectory(apps)
endif()

# Organize source files into folders for Visual Studio
source_group("Canvas" FILES src/framework/canvas/Canvas.cpp src/framework/canvas/MCanvas.cpp src/framework/canvas/MCanvas.h)
source_group("Rendering" FILES src/rendering/Graphics.cpp)
source_group("CodeEditor" FILES addons/bxCodeEditor/bxCodeEditor.cpp addons/bxCodeEditor/bxCodeEditor.h)
source_group("ThirdParty\\ImGuiColorTextEdit" FILES third_party/ImGuiColorTextEdit/TextEditor.cpp third_party/ImGuiColorTextEdit/TextEditor.h)
source_group("ThirdParty\\ImGui" FILES
    addons/bxImGui/imgui/imgui.cpp
    addons/bxImGui/imgui/imgui.h
    addons/bxImGui/imgui/imgui_draw.cpp
    addons/bxImGui/imgui/imgui_internal.h
    addons/bxImGui/imgui/imgui_tables.cpp
    addons/bxImGui/imgui/imgui_widgets.cpp
    addons/bxImGui/imgui/imgui_demo.cpp
    addons/bxImGui/imgui/imconfig.h
    addons/bxImGui/imgui/imstb_rectpack.h
    addons/bxImGui/imgui/imstb_textedit.h
    addons/bxImGui/imgui/imstb_truetype.h
    addons/bxImGui/imgui/backends/imgui_impl_glfw.cpp
    addons/bxImGui/imgui/backends/imgui_impl_glfw.h
    addons/bxImGui/imgui/backends/imgui_impl_opengl3.cpp
    addons/bxImGui/imgui/backends/imgui_impl_opengl3.h
    addons/bxImGui/imgui/backends/imgui_impl_opengl3_loader.h
)
# GLOB all Addons sources and headers
file(GLOB ADDONS_SOURCES src/addons/*.cpp src/addons/*.h)
source_group("Addons" FILES ${ADDONS_SOURCES})

# GLOB all Resources sources and headers
file(GLOB RESOURCES_SOURCES src/resources/*.cpp src/resources/*.h)
source_group("Resources" FILES ${RESOURCES_SOURCES})

# GLOB all ECS sources and headers
file(GLOB ECS_SOURCES src/ecs/*.cpp src/ecs/*.h src/ecs/systems/*.cpp src/ecs/systems/*.h)
source_group("ECS" FILES ${ECS_SOURCES})

# GLOB all Node sources and headers
source_group("Node" FILES src/ui/windows/NodeEditorWindow.cpp src/ui/windows/NodeEditorWindow.h)

# GLOB all Scripting sources and headers
file(GLOB SCRIPTING_SOURCES src/scripting/*.cpp src/scripting/*.h)
source_group("Scripting" FILES ${SCRIPTING_SOURCES})

# Use the globbed files in the source group and target sources
source_group("UI" FILES src/ui/ImGuiRenderer.cpp src/ui/CoordinateSystem.cpp ${UI_WINDOW_SOURCES} src/ui/Mui.cpp)
source_group("ECS" FILES src/ecs/MEcs.cpp src/ecs/systems/CanvasSystems.cpp src/ecs/systems/SEvent.cpp)
source_group("Node" FILES src/ui/windows/NodeEditorWindow.cpp src/ui/windows/NodeEditorWindow.h)
source_group("Scripting" FILES src/scripting/ScriptEngine.cpp) 

# Propagate third-party include directories needed by external consumers (apps)
target_include_directories(blot PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/third_party/glfw/include
    ${CMAKE_SOURCE_DIR}/third_party/spdlog/include
)

# add_subdirectory(works)  # Removed: directory no longer exists

add_subdirectory(third_party/glfw) 

target_include_directories(blot PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/rendering
    ${CMAKE_SOURCE_DIR}/src/ecs
    ${CMAKE_SOURCE_DIR}/src/core/canvas
    ${CMAKE_SOURCE_DIR}/addons/bxImGui/src
    # ...add others as needed
) 
