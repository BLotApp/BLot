# Addons required by sample_menubar
include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

# UI addon provided by parent directory
CPMAddPackage(
    NAME bxImGui
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../../..
)   
