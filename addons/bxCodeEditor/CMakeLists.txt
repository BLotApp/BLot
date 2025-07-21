cmake_minimum_required(VERSION 3.10)

set(ADDON_NAME "bxCodeEditor")

# Collect sources from src/
file(GLOB BXCODEEDITOR_SRC
    src/*.cpp
    src/*.h
)

add_library(${ADDON_NAME} STATIC ${BXCODEEDITOR_SRC})

# Depend on blot core and bxImGui (already fetched via addons.cmake)
target_link_libraries(${ADDON_NAME} PUBLIC blot bxImGui bxScriptEngine)

# Include directories
 target_include_directories(${ADDON_NAME} PUBLIC
     $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
     $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/third_party/ImGuiColorTextEdit>
 )
