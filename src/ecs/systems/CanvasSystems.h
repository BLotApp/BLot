#pragma once

#include "ecs/ECSManager.h"
#include "rendering/RenderingManager.h"
#include <entt/entity/entity.hpp>

namespace blot {
namespace systems {

void CanvasUpdateSystem(ECSManager& ecs, RenderingManager* renderingManager, float deltaTime);
void CanvasRenderSystem(ECSManager& ecs, RenderingManager* renderingManager, entt::entity activeCanvasId);

} // namespace systems
} // namespace blot 