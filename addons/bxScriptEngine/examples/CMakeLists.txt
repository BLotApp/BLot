file(GLOB EXAMPLE_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)

foreach(ex ${EXAMPLE_DIRS})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${ex}" AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ex}/CMakeLists.txt")
        add_subdirectory(${ex})
    endif()
endforeach() 