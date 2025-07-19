#include <memory>
#include <string>
#include <vector>
#include "rendering/rendering.h"

std::shared_ptr<IRenderer> createRenderer(RendererType type) {
	return RendererRegistry::instance().create(type);
}

// Helper function to get renderer type from string
RendererType getRendererTypeFromString(const std::string &name) {
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
