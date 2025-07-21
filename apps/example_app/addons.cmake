# Addons required by example_app

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/BlotAddon.cmake)

blot_addon_local_or_remote(bxImGui       BLotApp/bxImGui       main)
blot_addon_local_or_remote(bxScriptEngine BLotApp/bxScriptEngine main)
blot_addon_local_or_remote(bxCodeEditor   BLotApp/bxCodeEditor   main) 
