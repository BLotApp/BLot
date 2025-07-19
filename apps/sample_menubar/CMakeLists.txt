cmake_minimum_required(VERSION 3.10)
project(sample_menubar)

file(GLOB SAMPLE_MENUBAR_SOURCES *.cpp *.h)

add_executable(sample_menubar ${SAMPLE_MENUBAR_SOURCES})

target_link_libraries(sample_menubar PRIVATE blot) 