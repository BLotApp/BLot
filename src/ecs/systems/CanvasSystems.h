#pragma once

#include <entt/entt.hpp>
#include "ecs/ECSManager.h"
#include "rendering/RenderingManager.h"

namespace blot {
namespace systems {

void CanvasUpdateSystem(ECSManager &ecs, RenderingManager *renderingManager,
						float deltaTime);
void CanvasRenderSystem(ECSManager &ecs, RenderingManager *renderingManager,
						entt::entity activeCanvasId);

} // namespace systems
} // namespace blot
