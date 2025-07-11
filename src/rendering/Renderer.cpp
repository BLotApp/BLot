#include <memory>
#include <map>
#include "rendering/Renderer.h"
#include "rendering/Blend2DRenderer.h"

std::unique_ptr<IRenderer> createRenderer(RendererType type) {
    switch (type) {
        //case RendererType::OpenGL:
        //    return std::make_unique<OpenGLRenderer>();
        case RendererType::Blend2D:
            return std::make_unique<Blend2DRenderer>();
        default:
            return nullptr;
    }
}

// Helper function to get renderer type from string
RendererType getRendererTypeFromString(const std::string& name) {
    if (name == "opengl" || name == "OpenGL") {
        return RendererType::OpenGL;
    } else if (name == "blend2d" || name == "Blend2D") {
        return RendererType::Blend2D;
    }
    return RendererType::OpenGL; // Default
}

// Helper function to get all available renderer names
std::vector<std::string> getAvailableRendererNames() {
    return {"OpenGL", "Blend2D"};
} 