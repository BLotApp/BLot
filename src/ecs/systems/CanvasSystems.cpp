#include "CanvasSystems.h"

namespace blot {
namespace systems {

void CanvasUpdateSystem(ECSManager& ecs, ResourceManager* resourceManager, float deltaTime) {
    auto view = ecs.view<TextureComponent>();
    for (auto entity : view) {
        auto canvasPtr = resourceManager->getCanvas(entity);
        if (canvasPtr && *canvasPtr) {
            (*canvasPtr)->update(deltaTime);
        }
    }
}

void CanvasRenderSystem(ECSManager& ecs, ResourceManager* resourceManager, entt::entity activeCanvasId) {
    if (activeCanvasId != entt::null && ecs.hasComponent<TextureComponent>(activeCanvasId)) {
        auto canvasPtr = resourceManager->getCanvas(activeCanvasId);
        if (canvasPtr && *canvasPtr) {
            (*canvasPtr)->render();
        }
    }
}

} // namespace systems
} // namespace blot 