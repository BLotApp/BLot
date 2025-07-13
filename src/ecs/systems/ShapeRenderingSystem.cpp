#include "ShapeRenderingSystem.h"
#include "imgui.h"
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

ShapeRenderingSystem::ShapeRenderingSystem(std::shared_ptr<Blend2DRenderer> renderer)
    : m_renderer(renderer) {
}

ShapeRenderingSystem::~ShapeRenderingSystem() {
}

void ShapeRenderingSystem::renderShapes(ECSManager& ecs) {
    auto view = ecs.view<Transform, Shape, Style>();
    
    for (auto entity : view) {
        auto& transform = view.get<Transform>(entity);
        auto& shape = view.get<Shape>(entity);
        auto& style = view.get<Style>(entity);
        
        switch (shape.type) {
            case blot::components::Shape::Type::Rectangle:
                renderRectangle(transform, shape, style);
                break;
            case blot::components::Shape::Type::Ellipse:
                renderEllipse(transform, shape, style);
                break;
            case blot::components::Shape::Type::Line:
                renderLine(transform, shape, style);
                break;
            case blot::components::Shape::Type::Polygon:
                renderPolygon(transform, shape, style);
                break;
            case blot::components::Shape::Type::Star:
                renderStar(transform, shape, style);
                break;
        }
    }
}

void ShapeRenderingSystem::renderRectangle(const Transform& transform, const Shape& shape, const Style& style) {
    if (!m_renderer) return;
    
    float x = transform.x + shape.x1;
    float y = transform.y + shape.y1;
    float width = shape.x2 - shape.x1;
    float height = shape.y2 - shape.y1;
    
    // Apply transform
    x *= transform.scaleX;
    y *= transform.scaleY;
    width *= transform.scaleX;
    height *= transform.scaleY;
    
    if (style.hasFill) {
        setFillStyle(style);
        m_renderer->drawRect(x, y, width, height);
    }
    
    if (style.hasStroke) {
        setStrokeStyle(style);
        m_renderer->drawRect(x, y, width, height);
    }
}

void ShapeRenderingSystem::renderEllipse(const Transform& transform, const Shape& shape, const Style& style) {
    if (!m_renderer) return;
    
    float x = transform.x + shape.x1;
    float y = transform.y + shape.y1;
    float width = shape.x2 - shape.x1;
    float height = shape.y2 - shape.y1;
    
    // Apply transform
    x *= transform.scaleX;
    y *= transform.scaleY;
    width *= transform.scaleX;
    height *= transform.scaleY;
    
    float centerX = x + width * 0.5f;
    float centerY = y + height * 0.5f;
    float radiusX = width * 0.5f;
    float radiusY = height * 0.5f;
    
    if (style.hasFill) {
        setFillStyle(style);
        m_renderer->drawEllipse(centerX, centerY, radiusX, radiusY);
    }
    
    if (style.hasStroke) {
        setStrokeStyle(style);
        m_renderer->drawEllipse(centerX, centerY, radiusX, radiusY);
    }
}

void ShapeRenderingSystem::renderLine(const Transform& transform, const Shape& shape, const Style& style) {
    if (!m_renderer || !style.hasStroke) return;
    
    float x1 = transform.x + shape.x1;
    float y1 = transform.y + shape.y1;
    float x2 = transform.x + shape.x2;
    float y2 = transform.y + shape.y2;
    
    // Apply transform
    x1 *= transform.scaleX;
    y1 *= transform.scaleY;
    x2 *= transform.scaleX;
    y2 *= transform.scaleY;
    
    setStrokeStyle(style);
    m_renderer->drawLine(x1, y1, x2, y2);
}

void ShapeRenderingSystem::renderPolygon(const Transform& transform, const Shape& shape, const Style& style) {
    if (!m_renderer) return;
    
    float centerX = transform.x + shape.x1;
    float centerY = transform.y + shape.y1;
    float radius = shape.x2 - shape.x1;
    
    // Apply transform
    centerX *= transform.scaleX;
    centerY *= transform.scaleY;
    radius *= transform.scaleX;
    
    std::vector<float> points;
    int sides = shape.sides;
    for (int i = 0; i < sides; ++i) {
        float angle = 2.0f * M_PI * i / sides;
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);
        points.push_back(x);
        points.push_back(y);
    }
    
    std::vector<glm::vec2> glmPoints;
    for (size_t i = 0; i < points.size(); i += 2) {
        glmPoints.push_back(glm::vec2(points[i], points[i + 1]));
    }
    if (style.hasFill) {
        setFillStyle(style);
        m_renderer->drawPolygon(glmPoints);
    }
    
    if (style.hasStroke) {
        setStrokeStyle(style);
        m_renderer->drawPolygon(glmPoints);
    }
}

void ShapeRenderingSystem::renderStar(const Transform& transform, const Shape& shape, const Style& style) {
    if (!m_renderer) return;
    
    float centerX = transform.x + shape.x1;
    float centerY = transform.y + shape.y1;
    float outerRadius = shape.x2 - shape.x1;
    float innerRadius = outerRadius * shape.innerRadius;
    
    // Apply transform
    centerX *= transform.scaleX;
    centerY *= transform.scaleY;
    outerRadius *= transform.scaleX;
    innerRadius *= transform.scaleX;
    
    std::vector<float> points;
    int points_count = shape.sides * 2;
    for (int i = 0; i < points_count; ++i) {
        float angle = 2.0f * M_PI * i / points_count;
        float radius = (i % 2 == 0) ? outerRadius : innerRadius;
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);
        points.push_back(x);
        points.push_back(y);
    }
    
    std::vector<glm::vec2> glmPoints;
    for (size_t i = 0; i < points.size(); i += 2) {
        glmPoints.push_back(glm::vec2(points[i], points[i + 1]));
    }
    if (style.hasFill) {
        setFillStyle(style);
        m_renderer->drawPolygon(glmPoints);
    }
    
    if (style.hasStroke) {
        setStrokeStyle(style);
        m_renderer->drawPolygon(glmPoints);
    }
}

void ShapeRenderingSystem::renderSelectionOverlay(ECSManager& ecs, const ImVec2& canvasPos, const ImVec2& canvasSize) {
    auto view = ecs.view<Transform, Shape, Selection>();
    
    for (auto entity : view) {
        auto& transform = view.get<Transform>(entity);
        auto& shape = view.get<Shape>(entity);
        auto& selection = view.get<Selection>(entity);
        
        if (!selection.isSelected) continue;
        
        // Draw selection rectangle
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 pos(canvasPos.x + transform.x, canvasPos.y + transform.y);
        ImVec2 size(shape.x2 - shape.x1, shape.y2 - shape.y1);
        
        drawList->AddRect(
            pos,
            ImVec2(pos.x + size.x, pos.y + size.y),
            IM_COL32(0, 255, 0, 255),
            0.0f, 0, 2.0f
        );
    }
}

void ShapeRenderingSystem::renderDrawingPreview(ECSManager& ecs, const ImVec2& canvasPos, const ImVec2& canvasSize) {
    auto view = ecs.view<Transform, Shape, Drawing>();
    
    for (auto entity : view) {
        auto& transform = view.get<Transform>(entity);
        auto& shape = view.get<Shape>(entity);
        auto& drawing = view.get<Drawing>(entity);
        
        if (!drawing.isActive) continue;
        
        // Draw preview
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 start(canvasPos.x + drawing.startPos.x, canvasPos.y + drawing.startPos.y);
        ImVec2 current(canvasPos.x + drawing.currentPos.x, canvasPos.y + drawing.currentPos.y);
        
        drawList->AddRect(
            start,
            current,
            IM_COL32(255, 255, 0, 128),
            0.0f, 0, 1.0f
        );
    }
}

void ShapeRenderingSystem::setFillStyle(const Style& style) {
    if (!m_renderer) return;
    
    glm::vec4 color(style.fillR, style.fillG, style.fillB, style.fillA);
    m_renderer->setFillColor(color);
}

void ShapeRenderingSystem::setStrokeStyle(const Style& style) {
    if (!m_renderer) return;
    
    glm::vec4 color(style.strokeR, style.strokeG, style.strokeB, style.strokeA);
    m_renderer->setStrokeColor(color);
    m_renderer->setStrokeWidth(style.strokeWidth);
}

void ShapeRenderingSystem::convertColor(float r, float g, float b, float a, uint32_t& color) {
    uint8_t red = static_cast<uint8_t>(r * 255.0f);
    uint8_t green = static_cast<uint8_t>(g * 255.0f);
    uint8_t blue = static_cast<uint8_t>(b * 255.0f);
    uint8_t alpha = static_cast<uint8_t>(a * 255.0f);
    
    color = (alpha << 24) | (blue << 16) | (green << 8) | red;
} 