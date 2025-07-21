cmake_minimum_required(VERSION 3.10)
project(example_markdown)

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

CPMAddPackage(
    NAME bxMarkdown
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../..
)

file(GLOB SRC *.cpp *.h)
add_executable(example_markdown ${SRC})

target_link_libraries(example_markdown PRIVATE blot bxMarkdown)

target_include_directories(example_markdown PRIVATE ${CMAKE_SOURCE_DIR}) 
