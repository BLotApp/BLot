cmake_minimum_required(VERSION 3.10)

# ------------------------------------------------------------------
# Resolve dependencies
# ------------------------------------------------------------------
include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

CPMAddPackage(
    NAME bxImGui
    GITHUB_REPOSITORY BLotApp/bxImGui
    GIT_TAG main
)

CPMAddPackage(
    NAME bxScriptEngine
    GITHUB_REPOSITORY BLotApp/bxScriptEngine
    GIT_TAG main
)

add_library(bxCodeEditor STATIC
    bxCodeEditor.cpp
    bxCodeEditor.h
)

target_link_libraries(bxCodeEditor PUBLIC blot bxImGui bxScriptEngine)

target_include_directories(bxCodeEditor PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    ${CMAKE_SOURCE_DIR}/third_party/ImGuiColorTextEdit
)

message(STATUS "Configured addon: bxCodeEditor (auto-fetches bxImGui, bxScriptEngine)") 
