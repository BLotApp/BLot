# BlotAddons.cmake
#
# Usage:
#   set(APP_MANIFEST "${CMAKE_CURRENT_SOURCE_DIR}/apps/example_app/app.json")
#   include(cmake/BlotAddons.cmake)
#   blot_load_addons(${APP_MANIFEST} BLOT_ADDON_LIBS)
#   target_link_libraries(example_app PRIVATE ${BLOT_ADDON_LIBS})
#
# Requires CMake >= 3.19 for file(JSON ...)

function(blot_load_addons MANIFEST_PATH OUT_VAR)
    if(NOT EXISTS ${MANIFEST_PATH})
        message(FATAL_ERROR "Manifest file not found: ${MANIFEST_PATH}")
    endif()

    if(${CMAKE_VERSION} VERSION_LESS "3.19")
        message(FATAL_ERROR "BlotAddons.cmake requires CMake 3.19 or newer for JSON parsing, found ${CMAKE_VERSION}.")
    endif()

    message(STATUS "### Running CMake ${CMAKE_VERSION} (${CMAKE_COMMAND})")
    # Read the manifest JSON text into a variable and parse it with
    # string(JSON) (file(JSON ...) does not exist; string(JSON) was added
    # in CMake 3.19).
    file(READ "${MANIFEST_PATH}" _manifest)

    # Get length of dependencies array
    string(JSON _depsLen LENGTH "${_manifest}" dependencies)

    if(_depsLen EQUAL 0)
        message(STATUS "[BlotAddons] No dependencies listed in manifest ${MANIFEST_PATH}")
        set(${OUT_VAR} "" PARENT_SCOPE)
        return()
    endif()

    set(_addon_libs "")
    math(EXPR _lastIdx "${_depsLen}-1")
    foreach(idx RANGE 0 ${_lastIdx})
        string(JSON depName   GET "${_manifest}" dependencies ${idx} name)
        string(JSON depUrl    GET "${_manifest}" dependencies ${idx} url)
        string(JSON depBranch GET "${_manifest}" dependencies ${idx} branch)

        if(depName STREQUAL "")
            message(WARNING "[BlotAddons] Dependency entry ${idx} missing 'name'; skipping")
            continue()
        endif()

        CPMAddPackage(
            NAME ${depName}
            GITHUB_REPOSITORY ${depUrl}
            GIT_TAG ${depBranch}
            OPTIONS "ADD_SUBDIRECTORY"
        )

        list(APPEND _addon_libs ${depName})
    endforeach()

    list(REMOVE_DUPLICATES _addon_libs)
    set(${OUT_VAR} ${_addon_libs} PARENT_SCOPE)
endfunction() 
