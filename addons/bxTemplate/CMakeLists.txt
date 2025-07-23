cmake_minimum_required(VERSION 3.19)

set(ADDON_NAME "bxTemplate")
add_library(${ADDON_NAME} STATIC ${ADDON_NAME}.cpp ${ADDON_NAME}.h)

target_link_libraries(${ADDON_NAME} PUBLIC blot)

target_include_directories(${ADDON_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

message(STATUS "Configured addon: ${ADDON_NAME}") 
