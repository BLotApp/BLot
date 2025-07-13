#pragma once
#include <vector>
#include <glm/glm.hpp>

struct Guide {
    float position;
    bool vertical; // true = vertical, false = horizontal
    glm::vec4 color;
};

// Unified component for both canvases and textures
struct TextureComponent {
    unsigned int textureId = 0;   // OpenGL/Vulkan/DirectX/etc. texture or FBO ID
    int width = 0;
    int height = 0;
    bool renderTarget = false;    // true if this is a canvas/FBO
    // Optionally: format, filtering, mipmaps, etc.
    // int format = GL_RGBA;
    // int filtering = GL_LINEAR;

    // View state fields (from old CanvasComponent)
    float zoom = 1.0f;
    glm::vec2 offset = glm::vec2(0.0f, 0.0f);
    bool showRulers = false;
    bool showGuides = false;
    std::vector<Guide> guides;
}; 