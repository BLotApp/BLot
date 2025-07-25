#pragma once

#include <glm/glm.hpp>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace blot {

namespace ecs {

struct CWindow {
	std::string name;
	bool isVisible = true;
	bool isFocused = false;
	int zOrder = 0;
};

struct CWindowTransform {
	glm::vec2 position = glm::vec2(0, 0);
	glm::vec2 size = glm::vec2(400, 300);
	glm::vec2 minSize = glm::vec2(100, 100);
	glm::vec2 maxSize = glm::vec2(FLT_MAX, FLT_MAX);
};

struct CWindowStyle {
	float alpha = 1.0f;
	int flags = 0;
	glm::vec4 backgroundColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
};

struct CWindowInput {
	bool handleMouse = true;
	bool handleKeyboard = true;
	bool closeOnEscape = true;
};

struct CWindowSettings {
	bool showInMenu = true;
	bool showByDefault = true;
	bool canBeHidden = true;
	bool canBeMoved = true;
	bool canBeResized = true;
	bool canBeFocused = true;
	std::string menuPath = "";
	std::string category = "General";
};

} // namespace ecs
} // namespace blot
