#pragma once
#include <glm/glm.hpp>

// Unified component for both canvases and textures
struct TextureComponent {
    unsigned int textureId = 0;   // OpenGL/Vulkan/DirectX/etc. texture or FBO ID
    int width = 0;
    int height = 0;
    bool renderTarget = false;    // true if this is a canvas/FBO
    // Optionally: format, filtering, mipmaps, etc.
    // int format = GL_RGBA;
    // int filtering = GL_LINEAR;
}; 