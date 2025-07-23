cmake_minimum_required(VERSION 3.19)

set(ADDON_NAME "bxNodeEditor")

# ------------------------------------------------------------------
# ImGui node-editor sources (pulled in via submodule)
# ------------------------------------------------------------------
file(GLOB NODE_EDITOR_SRC CONFIGURE_DEPENDS
    third_party/imgui-node-editor/*.cpp
)

file(GLOB NODE_WINDOW_SRC windows/NodeEditorWindow.cpp)

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

CPMAddPackage(
    NAME bxImGui
    GITHUB_REPOSITORY BLotApp/bxImGui
    GIT_TAG main
)

add_library(${ADDON_NAME} STATIC
    ${ADDON_NAME}.cpp
    ${ADDON_NAME}.h
    ${NODE_EDITOR_SRC}
    ${NODE_WINDOW_SRC}
)

# Dependencies
target_link_libraries(${ADDON_NAME} PUBLIC blot bxImGui)

# Public include paths: own folder + node-editor headers
target_include_directories(${ADDON_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/third_party/imgui-node-editor>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/windows>
)

message(STATUS "Configured addon: ${ADDON_NAME} (with imgui-node-editor)") 
