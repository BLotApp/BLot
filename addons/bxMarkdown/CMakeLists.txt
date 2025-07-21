cmake_minimum_required(VERSION 3.10)

set(ADDON_NAME "bxMarkdown")

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

CPMAddPackage(
    NAME bxImGui
    GITHUB_REPOSITORY BLotApp/bxImGui
    GIT_TAG main
)

# -----------------------------------------------------------------------------
# Optional examples
option(BUILD_BXMARKDOWN_EXAMPLES "Build examples in this addon" OFF)

if(BUILD_BXMARKDOWN_EXAMPLES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples")
    add_subdirectory(examples)
endif()

add_library(${ADDON_NAME} STATIC
    bxMarkdown.cpp
    bxMarkdown.h
)

target_link_libraries(${ADDON_NAME} PUBLIC blot bxImGui)

target_include_directories(${ADDON_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/third_party/imgui_markdown>
)

message(STATUS "Configured addon: ${ADDON_NAME} with imgui_markdown") 
