cmake_minimum_required(VERSION 3.10)
project(sample_blend2d_gradient)

add_executable(sample_blend2d_gradient
    main.cpp
    app.cpp
    app_entry.h
    ${CMAKE_SOURCE_DIR}/src/engine/BlotEngine.h
    ${CMAKE_SOURCE_DIR}/src/engine/BlotEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/framework/IApp.h
    ${CMAKE_SOURCE_DIR}/src/ecs/ECSManager.cpp
    ${CMAKE_SOURCE_DIR}/src/ecs/systems/CanvasSystems.cpp
    ${CMAKE_SOURCE_DIR}/src/ecs/systems/EventSystem.cpp
    ${CMAKE_SOURCE_DIR}/src/ecs/systems/ShapeRenderingSystem.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/Renderer.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/RenderingManager.cpp
    ${CMAKE_SOURCE_DIR}/src/framework/canvas/Canvas.cpp
    ${CMAKE_SOURCE_DIR}/src/framework/canvas/CanvasManager.cpp
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
    ${CMAKE_SOURCE_DIR}/src/addons
    ${CMAKE_SOURCE_DIR}/third_party/blend2d/src
    ${CMAKE_SOURCE_DIR}/third_party/spdlog/include
    ${CMAKE_SOURCE_DIR}/third_party/entt/single_include
    ${CMAKE_SOURCE_DIR}/third_party/entt/src
    ${CMAKE_SOURCE_DIR}/third_party/glad/include
    ${CMAKE_SOURCE_DIR}/third_party/glfw/include
)

target_link_libraries(sample_blend2d_gradient PRIVATE blend2d::blend2d opengl32 glfw) 