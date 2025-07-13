#pragma once

#include <vector>
#include <string>

namespace blot {
namespace components {

struct Shape {
    enum class Type { 
        Rectangle, 
        Ellipse, 
        Line, 
        Polygon, 
        Star 
    };
    
    Type type = Type::Rectangle;
    
    // Rectangle/Ellipse: x1, y1 = top-left, x2, y2 = bottom-right
    // Line: x1, y1, x2, y2
    float x1 = 0.0f, y1 = 0.0f, x2 = 100.0f, y2 = 100.0f;
    
    // For polygon/star
    int sides = 5;
    float innerRadius = 0.5f; // For star shapes
    
    // Utility methods
    float getWidth() const { return x2 - x1; }
    float getHeight() const { return y2 - y1; }
    void setPosition(float x, float y) { x1 = x; y1 = y; }
    void setSize(float width, float height) { x2 = x1 + width; y2 = y1 + height; }
};

} // namespace components
} // namespace blot 