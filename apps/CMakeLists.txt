# Build each app dir if it exists so configuration doesn’t fail when a template is absent.

file(GLOB APP_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)

foreach(app ${APP_DIRS})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${app}" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${app}/CMakeLists.txt")
        message(STATUS "Adding app subdirectory: ${app}")
        add_subdirectory(${app})
    endif()
endforeach() 