#include "SCanvas.h"
#include "ecs/components/CTexture.h"
#include "ecs/systems/SShapeRendering.h"

namespace blot {
namespace ecs {

void SCanvasUpdate(MEcs &ecs, MRendering *renderingManager, float deltaTime) {
	auto view = ecs.view<ecs::CTexture>();
	for (auto entity : view) {
		auto canvasPtr = renderingManager->getCanvas(entity);
		if (canvasPtr && *canvasPtr) {
			(*canvasPtr)->update(deltaTime);
		}
	}
}

void SCanvasRender(MEcs &ecs, MRendering *renderingManager,
				   entt::entity activeCanvasId) {
	if (activeCanvasId != entt::null &&
		ecs.hasComponent<ecs::CTexture>(activeCanvasId)) {
		auto canvasPtr = renderingManager->getCanvas(activeCanvasId);
		if (canvasPtr && *canvasPtr) {
			(*canvasPtr)->render();
		}
	}
}

} // namespace ecs
} // namespace blot
