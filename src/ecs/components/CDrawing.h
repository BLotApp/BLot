#pragma once
#include <imgui.h>

namespace blot {
namespace ecs {

struct CDrawing {
	bool isActive = false;
	bool isSelected = false;
	bool isHovered = false;
	ImVec2 startPos = ImVec2(0, 0);
	ImVec2 currentPos = ImVec2(0, 0);
	ImVec2 endPos = ImVec2(0, 0);
};

} // namespace ecs
} // namespace blot
