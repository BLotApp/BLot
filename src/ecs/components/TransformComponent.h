#pragma once
#include "../PropertyReflection.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace blot {
namespace components {

struct Transform {
	glm::vec3 position{0.0f, 0.0f, 0.0f};
	glm::quat rotation =
		glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // identity quaternion
	glm::vec3 scale{1.0f, 1.0f, 1.0f};

	// For property reflection, expose Euler angles (in radians)
	std::vector<sProp> GetProperties() {
		// Convert quaternion to Euler angles (YXZ order)
		glm::vec3 euler = glm::eulerAngles(rotation);
		return {{0, "Position X", EPT_FLOAT, &position.x},
				{1, "Position Y", EPT_FLOAT, &position.y},
				{2, "Position Z", EPT_FLOAT, &position.z},
				{3, "Rotation X (rad)", EPT_FLOAT, &euler.x},
				{4, "Rotation Y (rad)", EPT_FLOAT, &euler.y},
				{5, "Rotation Z (rad)", EPT_FLOAT, &euler.z},
				{6, "Scale X", EPT_FLOAT, &scale.x},
				{7, "Scale Y", EPT_FLOAT, &scale.y},
				{8, "Scale Z", EPT_FLOAT, &scale.z}};
	}
};

} // namespace components
} // namespace blot
