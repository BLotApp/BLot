#pragma once
#include <entt/entt.hpp>
#include <string>

namespace blot {
namespace components {

struct Parameter {
	std::string name;
	float value = 0.0f;
	float minValue = 0.0f;
	float maxValue = 1.0f;
	bool isConnected = false;
	entt::entity connectedTo = entt::null;
};

} // namespace components
} // namespace blot
