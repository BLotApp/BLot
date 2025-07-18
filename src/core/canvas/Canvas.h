#pragma once

// Standard library
#include <memory>
#include <vector>
#include <string>
#include <variant>

// Third-party
#include <glm/glm.hpp>

// Project headers
#include "rendering/IRenderer.h"
#include "core/ISettings.h"

namespace blot {

// Forward declarations
class BlotEngine;
class Graphics;
class ECSManager;

/**
 * @brief Settings/configuration for Canvas creation.
 */
struct CanvasSettings {
    int width = 800;
    int height = 600;
    float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f; // Default white
    int samples = 0; // Multisampling
    // Add more options as needed
};

/**
 * @brief Canvas: Main drawing surface for Blot, supports ECS and traditional drawing.
 *
 * Handles OpenGL resources via PIMPL, integrates with ECS for shape management,
 * and provides a user-facing API for drawing, transformations, and exporting.
 */
class Canvas : public ISettings {
public:
    Canvas(const CanvasSettings& settings, BlotEngine* engine = nullptr);
    ~Canvas();

    // Drawing API
    void resize(int width, int height);
    void clear();
    void clear(float r, float g, float b, float a = 1.0f);
    void background(float r, float g, float b, float a = 1.0f);
    void fill(float r, float g, float b, float a = 1.0f);
    void noFill();
    void stroke(float r, float g, float b, float a = 1.0f);
    void noStroke();
    void strokeWeight(float weight);
    void strokeCap(int cap);
    void strokeJoin(int join);
    void strokeDash(const std::vector<float>& dashes, float offset = 0.0f);
    void rect(float x, float y, float width, float height);
    void ellipse(float x, float y, float width, float height);
    void line(float x1, float y1, float x2, float y2);
    void triangle(float x1, float y1, float x2, float y2, float x3, float y3);
    void circle(float x, float y, float diameter);
    void text(const std::string& text, float x, float y);
    void textSize(float size);
    void textAlign(int align);
    void pushMatrix();
    void popMatrix();
    void translate(float x, float y);
    void rotate(float angle);
    void scale(float x, float y);
    void update(float deltaTime);
    void render();
    void saveFrame(const std::string& filename);
    void exportSVG(const std::string& filename);

    // ECS integration
    void setECSManager(ECSManager* ecs) { m_ecs = ecs; }
    ECSManager* getECSManager() const { return m_ecs; }
    void renderECSShapes();

    // Engine access
    void setEngine(BlotEngine* engine) { m_engine = engine; }
    BlotEngine* getEngine() const { return m_engine; }

    // Renderer management
    void switchRenderer(RendererType type);
    void setRenderer(std::unique_ptr<IRenderer> renderer);
    RendererType getRendererType() const;

    // Getters
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    std::shared_ptr<Graphics> getGraphics() { return m_graphics; }
    unsigned int getColorTexture() const;
    void setName(const std::string& name) { m_name = name; }
    std::string getName() const { return m_name; }

    // ISettings interface
    json getSettings() const override;
    void setSettings(const json& settings) override;

private:
    void initFramebuffer();
    void initShaders();

    int m_width;
    int m_height;
    std::string m_name;
    CanvasSettings m_settings;

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

    // ECS
    ECSManager* m_ecs = nullptr;

    // Engine
    BlotEngine* m_engine = nullptr;
};

} // namespace blot 