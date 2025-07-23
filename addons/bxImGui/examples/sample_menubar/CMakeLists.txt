cmake_minimum_required(VERSION 3.10)
project(sample_menubar)

# Pull in CPM and addon manifest
include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/addons.cmake)

file(GLOB SAMPLE_MENUBAR_SOURCES *.cpp *.h)

add_executable(sample_menubar ${SAMPLE_MENUBAR_SOURCES})

target_link_libraries(sample_menubar PRIVATE blot bxImGui) 
target_include_directories(sample_menubar PRIVATE ${CMAKE_SOURCE_DIR}) 
