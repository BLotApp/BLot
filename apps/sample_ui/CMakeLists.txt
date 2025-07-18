cmake_minimum_required(VERSION 3.10)
project(sample_ui)

file(GLOB SAMPLE_UI_SOURCES *.cpp *.h)

add_executable(sample_ui ${SAMPLE_UI_SOURCES})

target_link_libraries(sample_ui PRIVATE blot) 