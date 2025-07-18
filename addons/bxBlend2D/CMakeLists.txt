cmake_minimum_required(VERSION 3.10)
project(bxBlend2D)

add_library(bxBlend2D STATIC Blend2DRenderer.cpp bxBlend2DAddon.cpp)
target_include_directories(bxBlend2D PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/addons
    ${CMAKE_SOURCE_DIR}/addons/bxBlend2D
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/rendering
    ${CMAKE_SOURCE_DIR}/src/ecs
    ${CMAKE_SOURCE_DIR}/third_party/spdlog/include
    ${CMAKE_SOURCE_DIR}/third_party/glfw/include
    ${CMAKE_SOURCE_DIR}/third_party/glad/include
    ${CMAKE_SOURCE_DIR}/third_party/blend2d/src
)
target_link_libraries(bxBlend2D PUBLIC blend2d::blend2d) 