#include "ShapeRenderingSystem.h"
#include "imgui.h"
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace blot {
namespace systems {

void ShapeRenderingSystem(ECSManager& ecs, std::shared_ptr<Blend2DRenderer> renderer) {
    auto view = ecs.view<Transform, Shape, Style>();
    
    for (auto entity : view) {
        auto& transform = view.get<Transform>(entity);
        auto& shape = view.get<Shape>(entity);
        auto& style = view.get<Style>(entity);
        
        switch (shape.type) {
            case blot::components::Shape::Type::Rectangle:
                renderRectangle(transform, shape, style, renderer);
                break;
            case blot::components::Shape::Type::Ellipse:
                renderEllipse(transform, shape, style, renderer);
                break;
            case blot::components::Shape::Type::Line:
                renderLine(transform, shape, style, renderer);
                break;
            case blot::components::Shape::Type::Polygon:
                renderPolygon(transform, shape, style, renderer);
                break;
            case blot::components::Shape::Type::Star:
                renderStar(transform, shape, style, renderer);
                break;
        }
    }
}

void renderRectangle(const Transform& transform, const Shape& shape, const Style& style, std::shared_ptr<Blend2DRenderer> renderer) {
    if (!renderer) return;
    
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
        setFillStyle(style, renderer);
        renderer->drawRect(x, y, width, height);
    }
    
    if (style.hasStroke) {
        setStrokeStyle(style, renderer);
        renderer->drawRect(x, y, width, height);
    }
}

void renderEllipse(const Transform& transform, const Shape& shape, const Style& style, std::shared_ptr<Blend2DRenderer> renderer) {
    if (!renderer) return;
    
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
        setFillStyle(style, renderer);
        renderer->drawEllipse(centerX, centerY, radiusX, radiusY);
    }
    
    if (style.hasStroke) {
        setStrokeStyle(style, renderer);
        renderer->drawEllipse(centerX, centerY, radiusX, radiusY);
    }
}

void renderLine(const Transform& transform, const Shape& shape, const Style& style, std::shared_ptr<Blend2DRenderer> renderer) {
    if (!renderer || !style.hasStroke) return;
    
    float x1 = transform.x + shape.x1;
    float y1 = transform.y + shape.y1;
    float x2 = transform.x + shape.x2;
    float y2 = transform.y + shape.y2;
    
    // Apply transform
    x1 *= transform.scaleX;
    y1 *= transform.scaleY;
    x2 *= transform.scaleX;
    y2 *= transform.scaleY;
    
    setStrokeStyle(style, renderer);
    renderer->drawLine(x1, y1, x2, y2);
}

void renderPolygon(const Transform& transform, const Shape& shape, const Style& style, std::shared_ptr<Blend2DRenderer> renderer) {
    if (!renderer) return;
    
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
        setFillStyle(style, renderer);
        renderer->drawPolygon(glmPoints);
    }
    
    if (style.hasStroke) {
        setStrokeStyle(style, renderer);
        renderer->drawPolygon(glmPoints);
    }
}

void renderStar(const Transform& transform, const Shape& shape, const Style& style, std::shared_ptr<Blend2DRenderer> renderer) {
    if (!renderer) return;
    
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
        setFillStyle(style, renderer);
        renderer->drawPolygon(glmPoints);
    }
    
    if (style.hasStroke) {
        setStrokeStyle(style, renderer);
        renderer->drawPolygon(glmPoints);
    }
}

void renderSelectionOverlay(ECSManager& ecs, const ImVec2& canvasPos, const ImVec2& canvasSize, std::shared_ptr<Blend2DRenderer> renderer) {
    auto view = ecs.view<Transform, Shape, Selection>();
    
    for (auto entity : view) {
        auto& transform = view.get<Transform>(entity);
        auto& shape = view.get<Shape>(entity);
        auto& selection = view.get<Selection>(entity);
        
        // Draw selection rectangle
        float x = transform.x + shape.x1;
        float y = transform.y + shape.y1;
        float width = shape.x2 - shape.x1;
        float height = shape.y2 - shape.y1;
        
        // Apply transform
        x *= transform.scaleX;
        y *= transform.scaleY;
        width *= transform.scaleX;
        height *= transform.scaleY;
        
        // Convert to screen coordinates
        float screenX = canvasPos.x + x;
        float screenY = canvasPos.y + y;
        
        // Draw selection rectangle
        if (renderer) {
            renderer->setStrokeColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red
            renderer->setStrokeWidth(2.0f);
            renderer->drawRect(screenX - 2, screenY - 2, width + 4, height + 4);
        }
    }
}

void renderDrawingPreview(ECSManager& ecs, const ImVec2& canvasPos, const ImVec2& canvasSize, std::shared_ptr<Blend2DRenderer> renderer) {
    auto view = ecs.view<Transform, Shape, Style>();
    
    for (auto entity : view) {
        auto& transform = view.get<Transform>(entity);
        auto& shape = view.get<Shape>(entity);
        auto& style = view.get<Style>(entity);
        
        // Convert to screen coordinates
        float x = canvasPos.x + transform.x + shape.x1;
        float y = canvasPos.y + transform.y + shape.y1;
        
        // Apply transform
        x *= transform.scaleX;
        y *= transform.scaleY;
        
        // Draw preview (simplified)
        if (renderer && style.hasStroke) {
            setStrokeStyle(style, renderer);
            switch (shape.type) {
                case blot::components::Shape::Type::Rectangle:
                    renderer->drawRect(x, y, (shape.x2 - shape.x1) * transform.scaleX, (shape.y2 - shape.y1) * transform.scaleY);
                    break;
                case blot::components::Shape::Type::Ellipse:
                    renderer->drawEllipse(x + (shape.x2 - shape.x1) * 0.5f * transform.scaleX, 
                                        y + (shape.y2 - shape.y1) * 0.5f * transform.scaleY,
                                        (shape.x2 - shape.x1) * 0.5f * transform.scaleX,
                                        (shape.y2 - shape.y1) * 0.5f * transform.scaleY);
                    break;
                case blot::components::Shape::Type::Line:
                    renderer->drawLine(x, y, x + (shape.x2 - shape.x1) * transform.scaleX, y + (shape.y2 - shape.y1) * transform.scaleY);
                    break;
                default:
                    break;
            }
        }
    }
}

void setFillStyle(const Style& style, std::shared_ptr<Blend2DRenderer> renderer) {
    if (!renderer) return;
    
    glm::vec4 fillColor(style.fillR, style.fillG, style.fillB, style.fillA);
    renderer->setFillColor(fillColor);
}

void setStrokeStyle(const Style& style, std::shared_ptr<Blend2DRenderer> renderer) {
    if (!renderer) return;
    
    glm::vec4 strokeColor(style.strokeR, style.strokeG, style.strokeB, style.strokeA);
    renderer->setStrokeColor(strokeColor);
    renderer->setStrokeWidth(style.strokeWidth);
}

void convertColor(float r, float g, float b, float a, uint32_t& color) {
    uint8_t red = static_cast<uint8_t>(r * 255.0f);
    uint8_t green = static_cast<uint8_t>(g * 255.0f);
    uint8_t blue = static_cast<uint8_t>(b * 255.0f);
    uint8_t alpha = static_cast<uint8_t>(a * 255.0f);
    
    color = (alpha << 24) | (blue << 16) | (green << 8) | red;
}

} // namespace systems
} // namespace blot 