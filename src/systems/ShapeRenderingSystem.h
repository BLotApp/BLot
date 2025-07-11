#pragma once
#include "ecs/ECSManager.h"
#include "rendering/Blend2DRenderer.h"
#include <memory>

// ECS-based shape rendering system
class ShapeRenderingSystem {
public:
    ShapeRenderingSystem(std::shared_ptr<Blend2DRenderer> renderer);
    ~ShapeRenderingSystem();
    
    // Main rendering function
    void renderShapes(ECSManager& ecs);
    
    // Individual shape rendering
    void renderRectangle(const Transform& transform, const Shape& shape, const Style& style);
    void renderEllipse(const Transform& transform, const Shape& shape, const Style& style);
    void renderLine(const Transform& transform, const Shape& shape, const Style& style);
    void renderPolygon(const Transform& transform, const Shape& shape, const Style& style);
    void renderStar(const Transform& transform, const Shape& shape, const Style& style);
    
    // UI rendering for selection and preview
    void renderSelectionOverlay(ECSManager& ecs, const ImVec2& canvasPos, const ImVec2& canvasSize);
    void renderDrawingPreview(ECSManager& ecs, const ImVec2& canvasPos, const ImVec2& canvasSize);
    
private:
    std::shared_ptr<Blend2DRenderer> m_renderer;
    
    // Helper functions
    void setFillStyle(const Style& style);
    void setStrokeStyle(const Style& style);
    void convertColor(float r, float g, float b, float a, uint32_t& color);
}; 