cmake_minimum_required(VERSION 3.10)
project(example_app)

# Pull in CPM and addons list
include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/addons.cmake)

file(GLOB EXAMPLE_APP_SOURCES *.cpp *.h)

add_executable(example_app ${EXAMPLE_APP_SOURCES})

target_link_libraries(example_app PRIVATE blot bxCodeEditor bxScriptEngine bxImGui) 
target_include_directories(example_app PRIVATE ${CMAKE_SOURCE_DIR}) 
