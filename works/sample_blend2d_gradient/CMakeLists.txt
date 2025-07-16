cmake_minimum_required(VERSION 3.10)
project(sample_blend2d_gradient)

add_executable(sample_blend2d_gradient
    main.cpp
    work.cpp
    ${CMAKE_SOURCE_DIR}/src/framework/IWork.h
    ${CMAKE_SOURCE_DIR}/src/framework/FrameworkApp.h
    ${CMAKE_SOURCE_DIR}/src/framework/FrameworkApp.cpp
    ${CMAKE_SOURCE_DIR}/src/ecs/ECSManager.cpp
    ${CMAKE_SOURCE_DIR}/src/ecs/systems/CanvasSystems.cpp
    ${CMAKE_SOURCE_DIR}/src/ecs/systems/EventSystem.cpp
    ${CMAKE_SOURCE_DIR}/src/ecs/systems/ShapeRenderingSystem.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/Renderer.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/Blend2DRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/Graphics.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/RenderingManager.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/Canvas.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasManager.cpp
    ${CMAKE_SOURCE_DIR}/src/scripting/ScriptEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/addons/AddonManager.cpp
    ${CMAKE_SOURCE_DIR}/third_party/glad/src/gl.c
)

# Find Blend2D (assumes it's available in the project or system)
find_package(blend2d CONFIG REQUIRED)

target_include_directories(sample_blend2d_gradient PRIVATE
    ${CMAKE_SOURCE_DIR}/src/framework
    ${CMAKE_SOURCE_DIR}/src/ecs
    ${CMAKE_SOURCE_DIR}/src/ecs/systems
    ${CMAKE_SOURCE_DIR}/src/rendering
    ${CMAKE_SOURCE_DIR}/src/canvas
    ${CMAKE_SOURCE_DIR}/src/scripting
    ${CMAKE_SOURCE_DIR}/src/addons
    ${CMAKE_SOURCE_DIR}/third_party/blend2d/src
    ${CMAKE_SOURCE_DIR}/third_party/spdlog/include
    ${CMAKE_SOURCE_DIR}/third_party/entt/single_include
    ${CMAKE_SOURCE_DIR}/third_party/entt/src
    ${CMAKE_SOURCE_DIR}/third_party/glad/include
    ${CMAKE_SOURCE_DIR}/third_party/glfw/include
)

target_link_libraries(sample_blend2d_gradient PRIVATE blend2d::blend2d opengl32 glfw) 