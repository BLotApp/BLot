#pragma once

#include "ecs/ECSManager.h"
#include "ecs/components/ShapeComponent.h"
#include "ecs/components/StyleComponent.h"
#include "ecs/components/TransformComponent.h"
#include "rendering/IRenderer.h"
#include <imgui.h>
#include <memory>
#include <glm/glm.hpp>

namespace blot {
namespace systems {

// Main rendering function
void ShapeRenderingSystem(ECSManager& ecs, std::shared_ptr<IRenderer> renderer);

// Individual shape rendering functions
void renderRectangle(const components::Transform& transform, const components::Shape& shape, const components::Style& style, std::shared_ptr<IRenderer> renderer);
void renderEllipse(const components::Transform& transform, const components::Shape& shape, const components::Style& style, std::shared_ptr<IRenderer> renderer);
void renderLine(const components::Transform& transform, const components::Shape& shape, const components::Style& style, std::shared_ptr<IRenderer> renderer);
void renderPolygon(const components::Transform& transform, const components::Shape& shape, const components::Style& style, std::shared_ptr<IRenderer> renderer);
void renderStar(const components::Transform& transform, const components::Shape& shape, const components::Style& style, std::shared_ptr<IRenderer> renderer);

// UI rendering for selection and preview
void renderSelectionOverlay(ECSManager& ecs, const ImVec2& canvasPos, const ImVec2& canvasSize, std::shared_ptr<IRenderer> renderer);
void renderDrawingPreview(ECSManager& ecs, const ImVec2& canvasPos, const ImVec2& canvasSize, std::shared_ptr<IRenderer> renderer);

// Helper functions
void setFillStyle(const components::Style& style, std::shared_ptr<IRenderer> renderer);
void setStrokeStyle(const components::Style& style, std::shared_ptr<IRenderer> renderer);
void convertColor(float r, float g, float b, float a, uint32_t& color);

} // namespace systems
} // namespace blot 