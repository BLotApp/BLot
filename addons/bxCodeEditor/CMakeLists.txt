cmake_minimum_required(VERSION 3.10)

add_library(bxCodeEditor STATIC
    bxCodeEditor.cpp
    bxCodeEditor.h
)

target_link_libraries(bxCodeEditor PUBLIC blot)

target_include_directories(bxCodeEditor PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    ${CMAKE_SOURCE_DIR}/third_party/ImGuiColorTextEdit
)

message(STATUS "Configured addon: bxCodeEditor") 
