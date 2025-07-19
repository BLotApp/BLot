#pragma once
#include <string>
#include "../PropertyReflection.h"

namespace blot {
namespace ecs {

struct CCanvas {
	int width = 1024;
	int height = 768;
	std::string name = "Canvas";
	float backgroundR = 1.0f, backgroundG = 1.0f, backgroundB = 1.0f,
		  backgroundA = 1.0f;
	bool showGrid = true;
	float gridSpacing = 32.0f;

	std::vector<sProp> GetProperties() {
		return {{0, "Width", EPT_INT, &width},
				{1, "Height", EPT_INT, &height},
				{2, "Name", EPT_STRING, &name},
				{3, "Background R", EPT_FLOAT, &backgroundR},
				{4, "Background G", EPT_FLOAT, &backgroundG},
				{5, "Background B", EPT_FLOAT, &backgroundB},
				{6, "Background A", EPT_FLOAT, &backgroundA},
				{7, "Show Grid", EPT_BOOL, &showGrid},
				{8, "Grid Spacing", EPT_FLOAT, &gridSpacing}};
	}
};

} // namespace ecs
} // namespace blot
