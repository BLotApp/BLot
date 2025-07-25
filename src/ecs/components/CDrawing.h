#pragma once
#include <glm/glm.hpp>

namespace blot {
namespace ecs {

struct CDrawing {
	bool isActive = false;
	bool isSelected = false;
	bool isHovered = false;
	glm::vec2 startPos = glm::vec2(0, 0);
	glm::vec2 currentPos = glm::vec2(0, 0);
	glm::vec2 endPos = glm::vec2(0, 0);
};

} // namespace ecs
} // namespace blot
