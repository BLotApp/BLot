# Addon system CMake configuration

# This file sets up the addon build system
# Each addon can have its own CMakeLists.txt file

message(STATUS "Configuring addon system...")

# Add addon subdirectories
# (ofxGui removed)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ofxOsc)
    add_subdirectory(ofxOsc)
endif()

# Add more addons as needed
# if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ofxKinect)
#     add_subdirectory(ofxKinect)
# endif() 

add_subdirectory(bxBlend2D) 