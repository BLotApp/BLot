#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "../PropertyReflection.h"

namespace blot {
namespace ecs {

struct CGuide {
	float position;
	bool vertical; // true = vertical, false = horizontal
	glm::vec4 color;
};

} // namespace ecs
} // namespace blot
