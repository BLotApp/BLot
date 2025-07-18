cmake_minimum_required(VERSION 3.10)
project(appTemplate)

# Collect all sources in this directory automatically
file(GLOB APP_TEMPLATE_SOURCES *.cpp *.h)

# Build executable; you may rename the target when you copy the template
add_executable(appTemplate ${APP_TEMPLATE_SOURCES})

# Link against the core framework library
target_link_libraries(appTemplate PRIVATE blot) 