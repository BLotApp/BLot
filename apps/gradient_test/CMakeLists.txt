cmake_minimum_required(VERSION 3.19)
project(gradient_test)

file(GLOB SRC *.cpp *.h)

add_executable(${PROJECT_NAME} ${SRC})

# Link with blot and the bxBlend2D addon
target_link_libraries(${PROJECT_NAME} PRIVATE blot bxBlend2D)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}) 
