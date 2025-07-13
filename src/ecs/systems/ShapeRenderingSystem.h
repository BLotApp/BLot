#pragma once

#include "ecs/ECSManager.h"
#include "ecs/components/ShapeComponent.h"
#include "ecs/components/StyleComponent.h"
#include "ecs/components/TransformComponent.h"
#include "rendering/Blend2DRenderer.h"
#include <imgui.h>
#include <memory>
#include <glm/glm.hpp>

namespace blot {
namespace systems {

// Main rendering function
void ShapeRenderingSystem(ECSManager& ecs, std::shared_ptr<Blend2DRenderer> renderer);

// Individual shape rendering functions
void renderRectangle(const Transform& transform, const Shape& shape, const Style& style, std::shared_ptr<Blend2DRenderer> renderer);
void renderEllipse(const Transform& transform, const Shape& shape, const Style& style, std::shared_ptr<Blend2DRenderer> renderer);
void renderLine(const Transform& transform, const Shape& shape, const Style& style, std::shared_ptr<Blend2DRenderer> renderer);
void renderPolygon(const Transform& transform, const Shape& shape, const Style& style, std::shared_ptr<Blend2DRenderer> renderer);
void renderStar(const Transform& transform, const Shape& shape, const Style& style, std::shared_ptr<Blend2DRenderer> renderer);

// UI rendering for selection and preview
void renderSelectionOverlay(ECSManager& ecs, const ImVec2& canvasPos, const ImVec2& canvasSize, std::shared_ptr<Blend2DRenderer> renderer);
void renderDrawingPreview(ECSManager& ecs, const ImVec2& canvasPos, const ImVec2& canvasSize, std::shared_ptr<Blend2DRenderer> renderer);

// Helper functions
void setFillStyle(const Style& style, std::shared_ptr<Blend2DRenderer> renderer);
void setStrokeStyle(const Style& style, std::shared_ptr<Blend2DRenderer> renderer);
void convertColor(float r, float g, float b, float a, uint32_t& color);

} // namespace systems
} // namespace blot 