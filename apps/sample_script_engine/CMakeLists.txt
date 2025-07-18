cmake_minimum_required(VERSION 3.10)
project(sample_script_engine)

add_executable(sample_script_engine main.cpp app.h app_entry.h)

target_include_directories(sample_script_engine PRIVATE
    ${CMAKE_SOURCE_DIR}/addons/bxScriptEngine
    ${CMAKE_SOURCE_DIR}/src/framework
    ${CMAKE_SOURCE_DIR}/third_party/glad/include
    ${CMAKE_SOURCE_DIR}/third_party/glfw/include
)

target_link_libraries(sample_script_engine PRIVATE glfw)
if(WIN32)
    target_sources(sample_script_engine PRIVATE ${CMAKE_SOURCE_DIR}/third_party/glad/src/gl.c)
    target_link_libraries(sample_script_engine PRIVATE opengl32)
endif() 