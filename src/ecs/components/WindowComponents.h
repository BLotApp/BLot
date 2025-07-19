#pragma once

#include <imgui.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "windows/Window.h"

namespace blot {

struct WindowComponent {
	std::shared_ptr<Window> window;
	std::string name;
	bool isVisible = true;
	bool isFocused = false;
	int zOrder = 0;
};

struct WindowTransformComponent {
	ImVec2 position = ImVec2(0, 0);
	ImVec2 size = ImVec2(400, 300);
	ImVec2 minSize = ImVec2(100, 100);
	ImVec2 maxSize = ImVec2(FLT_MAX, FLT_MAX);
};

struct WindowStyleComponent {
	float alpha = 1.0f;
	int flags = 0;
	ImVec4 backgroundColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
};

struct WindowInputComponent {
	bool handleMouse = true;
	bool handleKeyboard = true;
	bool closeOnEscape = true;
};

struct WindowSettingsComponent {
	bool showInMenu = true;
	bool showByDefault = true;
	bool canBeHidden = true;
	bool canBeMoved = true;
	bool canBeResized = true;
	bool canBeFocused = true;
	std::string menuPath = "";
	std::string category = "General";
};

} // namespace blot
