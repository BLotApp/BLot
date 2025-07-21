# Application-specific addons for appTemplate
# Add more CPMAddPackage() blocks as your app grows

include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)

CPMAddPackage(
    NAME bxTemplate
    GITHUB_REPOSITORY BLotApp/bxTemplate
    GIT_TAG main
)

# bxTemplate target is now available for linking 
