#pragma once

#include <string>

namespace blot {
struct WindowSettings {
	int width = 1280;
	int height = 720;
	std::string title = "Blot App";
	bool fullscreen = false;
};
} // namespace blot
