cmake_minimum_required(VERSION 3.10)
project(bxblend2d_gradient_demo)

file(GLOB SRC *.cpp *.h)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} PRIVATE blot bxBlend2D blend2d::blend2d glfw) 