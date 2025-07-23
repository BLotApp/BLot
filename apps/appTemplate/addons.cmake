# Application-specific addons for appTemplate
# Add more CPMAddPackage() blocks as your app grows

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/BlotAddon.cmake)

# bxTemplate addon
blot_addon_local_or_remote(bxTemplate BLotApp/bxTemplate main)

# bxTemplate target is now available for linking 
