#pragma once

#include <entt/entt.hpp>
#include "ecs/MEcs.h"
#include "rendering/MRendering.h"

namespace blot {
namespace ecs {

void SCanvasUpdate(MEcs &ecs, MRendering *renderingManager, float deltaTime);
void SCanvasRender(MEcs &ecs, MRendering *renderingManager,
				   entt::entity activeCanvasId);

} // namespace ecs
} // namespace blot
