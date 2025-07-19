#pragma once
#include <entt/entt.hpp>
#include <string>

namespace blot {
namespace ecs {

struct CParameter {
	std::string name;
	float value = 0.0f;
	float minValue = 0.0f;
	float maxValue = 1.0f;
	bool isConnected = false;
	entt::entity connectedTo = entt::null;
};

} // namespace ecs
} // namespace blot
