#pragma once

#include "ecs/ECSManager.h"
#include "rendering/ResourceManager.h"
#include <entt/entity/entity.hpp>

namespace blot {
namespace systems {

void CanvasUpdateSystem(ECSManager& ecs, ResourceManager* resourceManager, float deltaTime);
void CanvasRenderSystem(ECSManager& ecs, ResourceManager* resourceManager, entt::entity activeCanvasId);

} // namespace systems
} // namespace blot 