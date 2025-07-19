cmake_minimum_required(VERSION 3.10)
project(example_app)

file(GLOB EXAMPLE_APP_SOURCES *.cpp *.h)

add_executable(example_app ${EXAMPLE_APP_SOURCES})

target_link_libraries(example_app PRIVATE blot) 
