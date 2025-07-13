#pragma once

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <variant>
#include "rendering/Renderer.h"

class Graphics;
class ECSManager; // Forward declaration

// Shape management is now handled by ECS. No Shape struct or shape list here.

class Canvas {
public:
    Canvas(int width, int height);
    Canvas(int width, int height, std::shared_ptr<Graphics> graphics);
    ~Canvas();
    
    void resize(int width, int height);
    void clear();
    void clear(float r, float g, float b, float a = 1.0f);
    
    // Drawing methods inspired by DrawBot and Processing
    void background(float r, float g, float b, float a = 1.0f);
    void fill(float r, float g, float b, float a = 1.0f);
    void noFill();
    void stroke(float r, float g, float b, float a = 1.0f);
    void noStroke();
    void strokeWeight(float weight);
    void strokeCap(int cap);
    void strokeJoin(int join);
    void strokeDash(const std::vector<float>& dashes, float offset = 0.0f);
    
    // Shape drawing
    void rect(float x, float y, float width, float height);
    void ellipse(float x, float y, float width, float height);
    void line(float x1, float y1, float x2, float y2);
    void triangle(float x1, float y1, float x2, float y2, float x3, float y3);
    void circle(float x, float y, float diameter);
    
    // Text rendering
    void text(const std::string& text, float x, float y);
    void textSize(float size);
    void textAlign(int align);
    
    // Transformations
    void pushMatrix();
    void popMatrix();
    void translate(float x, float y);
    void rotate(float angle);
    void scale(float x, float y);
    
    // Animation support
    void update(float deltaTime);
    void render();
    
    // Export functionality
    void saveFrame(const std::string& filename);
    void exportSVG(const std::string& filename);
    
    // Getters
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    std::shared_ptr<Graphics> getGraphics() { return m_graphics; }
    unsigned int getColorTexture() const;
    
    // Shape management moved to ECS system
    void renderECSShapes(); // New method to render ECS shapes
    
    // ECS integration
    void setECSManager(ECSManager* ecs) { m_ecs = ecs; }
    ECSManager* m_ecs = nullptr;
    
    // Renderer management
    void switchRenderer(RendererType type);
    void setRenderer(std::unique_ptr<IRenderer> renderer);
    RendererType getRendererType() const;
    
    // Name management
    void setName(const std::string& name) { m_name = name; }
    std::string getName() const { return m_name; }

private:
    void initFramebuffer();
    void initShaders();
    
    int m_width;
    int m_height;

    // PIMPL for OpenGL resources
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    // Graphics state
    std::shared_ptr<Graphics> m_graphics;
    
    // Transform stack
    std::vector<glm::mat4> m_matrixStack;
    glm::mat4 m_currentMatrix;
    
    // Drawing state
    bool m_hasFill;
    bool m_hasStroke;
    glm::vec4 m_fillColor;
    glm::vec4 m_strokeColor;
    float m_strokeWeight;
    float m_textSize;
    int m_textAlign;
    
    // Animation
    float m_time;
    float m_frameRate;
    int m_frameCount;
    
    // Name of the canvas
    std::string m_name;
}; 