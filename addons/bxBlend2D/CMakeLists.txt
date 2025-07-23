cmake_minimum_required(VERSION 3.19)

set(ADDON_NAME "bxBlend2D")

# Build local dependencies
add_subdirectory(third_party/asmjit EXCLUDE_FROM_ALL)
add_subdirectory(third_party/blend2d EXCLUDE_FROM_ALL)

add_library(${ADDON_NAME} STATIC
    bxBlend2d.cpp
    Blend2DRenderer.cpp
    Blend2DRenderer.h
)

target_link_libraries(${ADDON_NAME}
    PUBLIC
        blot
        blend2d
)

target_include_directories(${ADDON_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/third_party/blend2d/src>
)

message(STATUS "Configured addon: ${ADDON_NAME} with local Blend2D/AsmJit")

# -----------------------------------------------------------------------------
# Optional examples
option(BUILD_ADDON_EXAMPLES "Build examples in this addon" OFF)

if(BUILD_ADDON_EXAMPLES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples")
    add_subdirectory(examples)
endif() 

file(GLOB BLEND2D_WIN_SRC windows/*.cpp windows/*.h)

target_sources(${ADDON_NAME} PRIVATE ${BLEND2D_WIN_SRC})

target_include_directories(${ADDON_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/windows>
)

target_link_libraries(${ADDON_NAME} PUBLIC bxImGui) 
