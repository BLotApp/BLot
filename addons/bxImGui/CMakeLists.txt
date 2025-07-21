cmake_minimum_required(VERSION 3.10)

set(ADDON_NAME "bxImGui")

file(GLOB BXIMGUI_SRC
    src/*.cpp
    src/*.h
)

add_library(${ADDON_NAME} STATIC ${BXIMGUI_SRC})

# bxImGui depends on blot core
target_link_libraries(${ADDON_NAME} PUBLIC blot)

target_include_directories(${ADDON_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
    ${CMAKE_CURRENT_LIST_DIR}/imgui
    ${CMAKE_CURRENT_LIST_DIR}/imgui/backends
    ${CMAKE_SOURCE_DIR}/src
)

# ------------------------------------------------------------------
# Dear ImGui dependency (core + backends)
# ------------------------------------------------------------------

set(_imgui_dir "${CMAKE_CURRENT_LIST_DIR}/imgui")

if (NOT EXISTS "${_imgui_dir}/imgui.h")
    # Ensure git trusts the build/_deps working directory on Windows FAT/NTFS
    execute_process(COMMAND git config --global --add safe.directory "*"
                    ERROR_QUIET OUTPUT_QUIET)
    # Fetch from GitHub if submodule not present
    include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
    CPMAddPackage(
        NAME imgui
        GITHUB_REPOSITORY ocornut/imgui
        GIT_TAG docking
    )
    set(_imgui_dir ${imgui_SOURCE_DIR})
    # Mark the checkout as safe to avoid Git ownership warnings on some systems
    execute_process(COMMAND git config --global --add safe.directory ${_imgui_dir}
                    ERROR_QUIET OUTPUT_QUIET)
endif()

# Core ImGui sources
file(GLOB IMGUI_CORE_SRC
    ${_imgui_dir}/imgui.cpp
    ${_imgui_dir}/imgui_draw.cpp
    ${_imgui_dir}/imgui_tables.cpp
    ${_imgui_dir}/imgui_widgets.cpp
)

# Backends (GLFW + OpenGL3)
set(_imgui_backend_dir "${_imgui_dir}/backends")
set(IMGUI_BACKEND_SRC
    ${_imgui_backend_dir}/imgui_impl_glfw.cpp
    ${_imgui_backend_dir}/imgui_impl_opengl3.cpp
)

target_sources(${ADDON_NAME} PUBLIC ${IMGUI_CORE_SRC} ${IMGUI_BACKEND_SRC})

target_include_directories(${ADDON_NAME} PUBLIC
    ${_imgui_dir}
    ${_imgui_backend_dir}
)

# ------------------------------------------------------------------
# Local third-party libs (optional – only add if present)
# ------------------------------------------------------------------

set(_imGuizmo_dir "${CMAKE_CURRENT_LIST_DIR}/third_party/ImGuizmo")
set(_implot_dir   "${CMAKE_CURRENT_LIST_DIR}/third_party/implot")

if (EXISTS "${_imGuizmo_dir}/CMakeLists.txt")
    add_subdirectory(${_imGuizmo_dir} EXCLUDE_FROM_ALL)
    list(APPEND _third_party_libs ImGuizmo)
endif()

if (EXISTS "${_implot_dir}/CMakeLists.txt")
    add_subdirectory(${_implot_dir} EXCLUDE_FROM_ALL)
    list(APPEND _third_party_libs implot)
endif()

# Link any libraries that were added
if(_third_party_libs)
    target_link_libraries(${ADDON_NAME} PUBLIC ${_third_party_libs})
endif()

target_include_directories(${ADDON_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/third_party/IconFontCppHeaders>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/third_party/implot3d>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/third_party/imgui-filebrowser>
)

# Build examples for this addon (optional)
option(BUILD_BXIMGUI_EXAMPLES "Build bxImGui examples" OFF)
if(BUILD_BXIMGUI_EXAMPLES AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/examples")
    add_subdirectory(examples)
endif()

message(STATUS "Configured addon: ${ADDON_NAME}") 
