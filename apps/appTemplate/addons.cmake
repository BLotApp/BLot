# Application-specific addons for appTemplate
# Add more CPMAddPackage() blocks as your app grows

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/BlotAddon.cmake)

# Dependencies are now automatically loaded from app.json
# No need for hardcoded addon references here
