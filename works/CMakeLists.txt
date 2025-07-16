cmake_minimum_required(VERSION 3.10)

# Automatically add all subdirectories (works)
file(GLOB WORK_DIRS RELATIVE ${CMAKE_CURRENT_LIST_DIR} *)
foreach(dir ${WORK_DIRS})
    if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${dir})
        add_subdirectory(${dir})
    endif()
endforeach() 