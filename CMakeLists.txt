cmake_minimum_required(VERSION 3.16)

# vcpkg toolchain logic removed – project now builds entirely from vendored
# submodules and system libraries.

project(Blot VERSION 1.0.0 LANGUAGES C CXX)

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
include_directories(${CMAKE_SOURCE_DIR}/third_party/json/single_include)

# Remove legacy vcpkg references – Blend2D is provided as a submodule
# Add Blend2D submodule
add_subdirectory(third_party/blend2d)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party/imgui)
include_directories(${CMAKE_SOURCE_DIR}/third_party/imgui/backends)
include_directories(${CMAKE_SOURCE_DIR}/third_party/imgui-node-editor)
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
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/imgui_demo.cpp
)
# GLOB ImGui backends (only the ones you use)
file(GLOB IMGUI_BACKEND_SOURCES
    third_party/imgui/backends/imgui_impl_glfw.cpp
    third_party/imgui/backends/imgui_impl_opengl3.cpp
)
# GLOB ImGuiColorTextEdit
file(GLOB IMGUI_COLOR_TEXT_EDIT_SOURCES
    third_party/ImGuiColorTextEdit/TextEditor.cpp
)
# GLOB imgui-node-editor sources
file(GLOB IMGUI_NODE_EDITOR_SOURCES
    third_party/imgui-node-editor/imgui_node_editor.cpp
    third_party/imgui-node-editor/imgui_node_editor_api.cpp
    third_party/imgui-node-editor/imgui_canvas.cpp
    third_party/imgui-node-editor/crude_json.cpp
)

# GLOB addon source files (one .cpp per addon directory)
file(GLOB ADDON_SOURCE_FILES
    addons/*/*.cpp
)

add_library(blot STATIC ${BLOT_SOURCES})
target_sources(blot PRIVATE
    ${IMGUI_SOURCES}
    ${IMGUI_BACKEND_SOURCES}
    ${IMGUI_COLOR_TEXT_EDIT_SOURCES}
    ${IMGUI_NODE_EDITOR_SOURCES}
    ${ADDON_SOURCE_FILES}
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
        blend2d
)

# Add include directories for submodules (ImGui, etc.)
target_include_directories(blot PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third_party/imgui
    ${CMAKE_SOURCE_DIR}/third_party/imgui/backends
    ${CMAKE_SOURCE_DIR}/third_party/imgui-node-editor
    ${CMAKE_SOURCE_DIR}/third_party/json/single_include
    ${CMAKE_SOURCE_DIR}/third_party/imgui-filebrowser
    ${CMAKE_SOURCE_DIR}/third_party/ImGuiColorTextEdit
)

# Automatically add all addon subdirectories to the include path
if(EXISTS "${CMAKE_SOURCE_DIR}/addons")
    file(GLOB ADDON_DIRS RELATIVE "${CMAKE_SOURCE_DIR}/addons" "${CMAKE_SOURCE_DIR}/addons/*")
    foreach(addon ${ADDON_DIRS})
        if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/addons/${addon}")
            target_include_directories(blot PUBLIC "${CMAKE_SOURCE_DIR}/addons/${addon}")
        endif()
    endforeach()
    target_include_directories(blot PUBLIC "${CMAKE_SOURCE_DIR}/addons")
endif()

# Compiler flags
if(MSVC)
    target_compile_options(blot PRIVATE /W4)
else()
    target_compile_options(blot PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Copy assets and addons
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY addons DESTINATION ${CMAKE_BINARY_DIR})

# Add addon subdirectories if they exist
if(EXISTS ${CMAKE_SOURCE_DIR}/addons)
    add_subdirectory(addons)
endif() 

# -----------------------------------------------------------------------------
# Optional sample / demo applications
option(BUILD_APPS "Build sample applications in /apps" ON)
if(BUILD_APPS)
    add_subdirectory(apps)
endif()

# ImGuizmo integration
set(IMGUIZMO_DIR ${CMAKE_SOURCE_DIR}/third_party/ImGuizmo)
set(IMGUIZMO_SRC ${IMGUIZMO_DIR}/ImGuizmo.cpp)

add_library(ImGuizmo STATIC ${IMGUIZMO_SRC})
target_include_directories(ImGuizmo PUBLIC ${IMGUIZMO_DIR})

# Link ImGuizmo to your main app target (replace blot with your target name if different)
target_link_libraries(blot PRIVATE ImGuizmo) 

# Organize source files into folders for Visual Studio
source_group("Canvas" FILES src/framework/canvas/Canvas.cpp src/framework/canvas/CanvasManager.cpp src/framework/canvas/CanvasManager.h)
source_group("Rendering" FILES src/rendering/Graphics.cpp src/rendering/Renderer.cpp)
source_group("CodeEditor" FILES addons/bxCodeEditor/bxCodeEditor.cpp addons/bxCodeEditor/bxCodeEditor.h)
source_group("ThirdParty\\ImGuiColorTextEdit" FILES third_party/ImGuiColorTextEdit/TextEditor.cpp third_party/ImGuiColorTextEdit/TextEditor.h)
source_group("ThirdParty\\ImGui" FILES
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui.h
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_internal.h
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/imgui_demo.cpp
    third_party/imgui/imconfig.h
    third_party/imgui/imstb_rectpack.h
    third_party/imgui/imstb_textedit.h
    third_party/imgui/imstb_truetype.h
    third_party/imgui/backends/imgui_impl_glfw.cpp
    third_party/imgui/backends/imgui_impl_glfw.h
    third_party/imgui/backends/imgui_impl_opengl3.cpp
    third_party/imgui/backends/imgui_impl_opengl3.h
    third_party/imgui/backends/imgui_impl_opengl3_loader.h
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
file(GLOB NODE_SOURCES src/ui/windows/NodeEditorWindow.cpp src/ui/windows/NodeEditorWindow.h)
source_group("Node" FILES ${NODE_SOURCES})

# GLOB all Scripting sources and headers
file(GLOB SCRIPTING_SOURCES src/scripting/*.cpp src/scripting/*.h)
source_group("Scripting" FILES ${SCRIPTING_SOURCES})

# Use the globbed files in the source group and target sources
source_group("UI" FILES src/ui/ImGuiRenderer.cpp src/ui/CoordinateSystem.cpp ${UI_WINDOW_SOURCES} src/ui/UIManager.cpp src/ui/UIManager.h)
source_group("ECS" FILES src/ecs/ECSManager.cpp src/ecs/systems/CanvasSystems.cpp src/ecs/systems/EventSystem.cpp)
source_group("Node" FILES src/ui/windows/NodeEditorWindow.cpp src/ui/windows/NodeEditorWindow.h)
source_group("Scripting" FILES src/scripting/ScriptEngine.cpp) 

# spdlog is now included as a subdependency
add_subdirectory(third_party/spdlog)
target_link_libraries(blot PUBLIC spdlog::spdlog) 

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
    # ...add others as needed
) 
