cmake_minimum_required(VERSION 3.19)

set(ADDON_NAME "bxOsc")

file(GLOB BXOSC_SRC
    *.cpp
    *.h
)

add_library(${ADDON_NAME} STATIC ${BXOSC_SRC})

# bxOsc depends on blot core
target_link_libraries(${ADDON_NAME} PUBLIC blot)

target_include_directories(${ADDON_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    ${CMAKE_SOURCE_DIR}/src
)

message(STATUS "Configured addon: ${ADDON_NAME}") 
