# Addons required by example_app
include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

# Code editor addon
CPMAddPackage(
    NAME bxCodeEditor
    GITHUB_REPOSITORY BLotApp/bxCodeEditor
    GIT_TAG main
)

# Script engine addon
CPMAddPackage(
    NAME bxScriptEngine
    GITHUB_REPOSITORY BLotApp/bxScriptEngine
    GIT_TAG main
)

# UI addon
CPMAddPackage(
    NAME bxImGui
    GITHUB_REPOSITORY BLotApp/bxImGui
    GIT_TAG main
) 
