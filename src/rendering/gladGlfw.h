#pragma once

// Include GLAD first (OpenGL loader)
#include <glad/gl.h>

// Then include GLFW without system OpenGL headers
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// Now both GLAD and GLFW are available
// GLAD provides OpenGL function pointers
// GLFW provides window management and input handling 
