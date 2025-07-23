cmake_minimum_required(VERSION 3.10)
project(example_filebrowser)

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

CPMAddPackage(
    NAME bxImGui
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../..
)

file(GLOB SRC *.cpp *.h)
add_executable(example_filebrowser ${SRC})

target_link_libraries(example_filebrowser PRIVATE blot bxImGui)

target_include_directories(example_filebrowser PRIVATE ${CMAKE_SOURCE_DIR}) 
