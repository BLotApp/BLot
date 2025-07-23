cmake_minimum_required(VERSION 3.10)
project(appTemplate)

# Collect all sources in this directory automatically
file(GLOB APP_TEMPLATE_SOURCES *.cpp *.h)

# --------------------------------------------------------------------
# Per-app addons via CPM
# --------------------------------------------------------------------
include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/addons.cmake)

# Build executable; you may rename the target when you copy the template
add_executable(appTemplate ${APP_TEMPLATE_SOURCES})

# Link against the core framework library and required addons
target_link_libraries(appTemplate PRIVATE blot bxTemplate) 
