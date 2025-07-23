cmake_minimum_required(VERSION 3.19)

add_library(bxScriptEngine STATIC
    bxScriptEngine.cpp
    bxScriptEngine.h
)

target_link_libraries(bxScriptEngine PUBLIC blot)

target_include_directories(bxScriptEngine PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)

message(STATUS "Configured addon: bxScriptEngine")

# -----------------------------------------------------------------------------
# Optional examples
option(BUILD_ADDON_EXAMPLES "Build examples in this addon" OFF)

if(BUILD_ADDON_EXAMPLES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples")
    add_subdirectory(examples)
endif() 
