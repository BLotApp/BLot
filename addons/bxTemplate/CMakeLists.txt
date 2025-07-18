# Addon Template CMakeLists.txt
# Copy this to your new addon directory and modify as needed

set(ADDON_NAME "bxTemplate")
set(ADDON_SOURCES
    ${ADDON_NAME}.h
    ${ADDON_NAME}.cpp
)

# Create addon library
add_library(${ADDON_NAME} SHARED ${ADDON_SOURCES})

# Link with main application
target_link_libraries(${ADDON_NAME} blot)

# Set include directories
# Add all necessary include directories for third-party and project-root-relative includes
# (spdlog, glfw, glad, src, etc.)
target_include_directories(${ADDON_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/rendering
    ${CMAKE_SOURCE_DIR}/src/ecs
    ${CMAKE_SOURCE_DIR}/third_party/spdlog/include
    ${CMAKE_SOURCE_DIR}/third_party/glfw/include
    ${CMAKE_SOURCE_DIR}/third_party/glad/include
)

# Set output directory
set_target_properties(${ADDON_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/addons
)

message(STATUS "Built addon: ${ADDON_NAME}") 