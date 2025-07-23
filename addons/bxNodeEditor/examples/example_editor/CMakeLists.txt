cmake_minimum_required(VERSION 3.10)
project(example_editor)

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

# Fetch bxNodeEditor (which auto-fetches bxImGui)
CPMAddPackage(
    NAME bxNodeEditor
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../..
)

file(GLOB EXAMPLE_SRC *.cpp *.h)

add_executable(example_editor ${EXAMPLE_SRC})

target_link_libraries(example_editor PRIVATE blot bxNodeEditor)

target_include_directories(example_editor PRIVATE ${CMAKE_SOURCE_DIR}) 
