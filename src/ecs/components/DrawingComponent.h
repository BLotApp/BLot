#pragma once
#include <imgui.h>

namespace blot {
namespace components {

struct Drawing {
	bool isActive = false;
	bool isSelected = false;
	bool isHovered = false;
	ImVec2 startPos = ImVec2(0, 0);
	ImVec2 currentPos = ImVec2(0, 0);
	ImVec2 endPos = ImVec2(0, 0);
};

} // namespace components
} // namespace blot
