#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>

// Forward declarations
class Canvas;
class Graphics;

enum class RendererType {
    OpenGL,
    Blend2D
};

class IRenderer {
public:
    virtual ~IRenderer() = default;
    
    // Initialization
    virtual bool initialize(int width, int height) = 0;
    virtual void shutdown() = 0;
    virtual void resize(int width, int height) = 0;
    
    // Rendering state
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void clear(const glm::vec4& color) = 0;
    
    // Drawing primitives
    virtual void drawLine(float x1, float y1, float x2, float y2) = 0;
    virtual void drawRect(float x, float y, float width, float height) = 0;
    virtual void drawCircle(float x, float y, float radius) = 0;
    virtual void drawEllipse(float x, float y, float width, float height) = 0;
    virtual void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3) = 0;
    virtual void drawPolygon(const std::vector<glm::vec2>& points) = 0;
    
    // Path drawing
    virtual void beginPath() = 0;
    virtual void moveTo(float x, float y) = 0;
    virtual void lineTo(float x, float y) = 0;
    virtual void curveTo(float cx1, float cy1, float cx2, float cy2, float x, float y) = 0;
    virtual void closePath() = 0;
    virtual void fill(const glm::vec4& color) = 0;
    virtual void stroke(const glm::vec4& color, float width) = 0;
    
    // Text rendering
    virtual void setFont(const std::string& fontPath, float size) = 0;
    virtual void drawText(const std::string& text, float x, float y, const glm::vec4& color) = 0;
    virtual glm::vec2 getTextBounds(const std::string& text) = 0;
    
    // Transformations
    virtual void pushMatrix() = 0;
    virtual void popMatrix() = 0;
    virtual void translate(float x, float y) = 0;
    virtual void rotate(float angle) = 0;
    virtual void scale(float sx, float sy) = 0;
    virtual void resetMatrix() = 0;
    
    // State setters
    virtual void setFillColor(const glm::vec4& color) = 0;
    virtual void setStrokeColor(const glm::vec4& color) = 0;
    virtual void setStrokeWidth(float width) = 0;
    
    // Export
    virtual bool saveToFile(const std::string& filename) = 0;
    virtual bool saveToMemory(std::vector<uint8_t>& data) = 0;
    
    // Getters
    virtual RendererType getType() const = 0;
    virtual std::string getName() const = 0;
    virtual bool isInitialized() const = 0;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual uint8_t* getPixelBuffer() = 0;
};

// Factory function
std::shared_ptr<IRenderer> createRenderer(RendererType type); 