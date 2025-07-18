add_library(bxScriptEngine
    bxScriptEngine.cpp
    bxScriptEngine.h
)
target_include_directories(bxScriptEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/rendering
    ${CMAKE_SOURCE_DIR}/src/ecs
    ${CMAKE_SOURCE_DIR}/third_party/spdlog/include
    ${CMAKE_SOURCE_DIR}/third_party/glfw/include
    ${CMAKE_SOURCE_DIR}/third_party/glad/include
) 