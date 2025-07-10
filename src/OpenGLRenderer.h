#pragma once

#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include <map>
#include "VertexBuffer.h"

class OpenGLRenderer : public IRenderer {
public:
    OpenGLRenderer();
    ~OpenGLRenderer() override;
    
    // IRenderer implementation
    bool initialize(int width, int height) override;
    void shutdown() override;
    void resize(int width, int height) override;
    
    void beginFrame() override;
    void endFrame() override;
    void clear(const glm::vec4& color) override;
    
    void drawLine(float x1, float y1, float x2, float y2, float strokeWidth, const glm::vec4& color) override;
    void drawRect(float x, float y, float width, float height, const glm::vec4& fillColor, const glm::vec4& strokeColor, float strokeWidth) override;
    void drawCircle(float x, float y, float radius, const glm::vec4& fillColor, const glm::vec4& strokeColor, float strokeWidth) override;
    void drawEllipse(float x, float y, float width, float height, const glm::vec4& fillColor, const glm::vec4& strokeColor, float strokeWidth) override;
    void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, const glm::vec4& fillColor, const glm::vec4& strokeColor, float strokeWidth) override;
    void drawPolygon(const std::vector<glm::vec2>& points, const glm::vec4& fillColor, const glm::vec4& strokeColor, float strokeWidth) override;
    
    void beginPath() override;
    void moveTo(float x, float y) override;
    void lineTo(float x, float y) override;
    void curveTo(float cx1, float cy1, float cx2, float cy2, float x, float y) override;
    void closePath() override;
    void fill(const glm::vec4& color) override;
    void stroke(const glm::vec4& color, float width) override;
    
    void setFont(const std::string& fontPath, float size) override;
    void drawText(const std::string& text, float x, float y, const glm::vec4& color) override;
    glm::vec2 getTextBounds(const std::string& text) override;
    
    void pushMatrix() override;
    void popMatrix() override;
    void translate(float x, float y) override;
    void rotate(float angle) override;
    void scale(float sx, float sy) override;
    void resetMatrix() override;
    
    bool saveToFile(const std::string& filename) override;
    bool saveToMemory(std::vector<uint8_t>& data) override;
    
    RendererType getType() const override { return RendererType::OpenGL; }
    std::string getName() const override { return "OpenGL"; }
    bool isInitialized() const override { return m_initialized; }

private:
    void setupShaders();
    void setupBuffers();
    void renderPath();
    
    bool m_initialized;
    int m_width, m_height;
    
    // PIMPL for OpenGL resources and text rendering
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    // Path rendering
    std::vector<glm::vec2> m_pathPoints;
    bool m_pathOpen;
    
    // Matrix stack
    std::vector<glm::mat4> m_matrixStack;
    glm::mat4 m_currentMatrix;
    
    // Rendering state
    glm::vec4 m_currentFillColor;
    glm::vec4 m_currentStrokeColor;
    float m_currentStrokeWidth;
}; 