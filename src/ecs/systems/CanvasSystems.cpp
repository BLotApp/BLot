#include "CanvasSystems.h"
#include "ecs/systems/ShapeRenderingSystem.h"
#include "ecs/components/TextureComponent.h"

namespace blot {
namespace systems {

void CanvasUpdateSystem(ECSManager& ecs, RenderingManager* renderingManager, float deltaTime) {
    auto view = ecs.view<components::TextureComponent>();
    for (auto entity : view) {
        auto canvasPtr = renderingManager->getCanvas(entity);
        if (canvasPtr && *canvasPtr) {
            (*canvasPtr)->update(deltaTime);
        }
    }
}

void CanvasRenderSystem(ECSManager& ecs, RenderingManager* renderingManager, entt::entity activeCanvasId) {
    if (activeCanvasId != entt::null && ecs.hasComponent<components::TextureComponent>(activeCanvasId)) {
        auto canvasPtr = renderingManager->getCanvas(activeCanvasId);
        if (canvasPtr && *canvasPtr) {
            (*canvasPtr)->render();
        }
    }
}

} // namespace systems
} // namespace blot 