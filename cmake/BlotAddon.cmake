# New file: cmake/BlotAddon.cmake
# Helper function to use local addon source if present, otherwise download via CPM
# Usage:
#   blot_addon_local_or_remote(<name> <repo> <tag>)
#
# <name>  - addon name (also directory name under ${CMAKE_SOURCE_DIR}/addons)
# <repo>  - GitHub repository in the form <owner>/<repo>
# <tag>   - git tag or branch to fetch when remote download is needed

function(blot_addon_local_or_remote NAME REPOSITORY TAG)
    set(_local_dir "${CMAKE_SOURCE_DIR}/addons/${NAME}")
    if (EXISTS "${_local_dir}/CMakeLists.txt")
        CPMAddPackage(NAME ${NAME} SOURCE_DIR ${_local_dir})
    else()
        CPMAddPackage(
            NAME ${NAME}
            GITHUB_REPOSITORY ${REPOSITORY}
            GIT_TAG ${TAG}
        )
    endif()
endfunction() 
