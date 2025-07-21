cmake_minimum_required(VERSION 3.10)
project(example_ImGuizmo)

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

CPMAddPackage(
    NAME bxImGui
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../..
)

file(GLOB SRC *.cpp *.h)
add_executable(example_ImGuizmo ${SRC})
target_link_libraries(example_ImGuizmo PRIVATE blot bxImGui)
target_include_directories(example_ImGuizmo PRIVATE ${CMAKE_SOURCE_DIR}) 
