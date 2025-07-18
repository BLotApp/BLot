cmake_minimum_required(VERSION 3.10)
project(bxscriptengine_simple_console)

file(GLOB SRC *.cpp *.h)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} PRIVATE blot bxScriptEngine) 