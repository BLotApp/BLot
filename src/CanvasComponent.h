#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "Canvas.h"
#include "Graphics.h"
#include "Blend2DRenderer.h"

struct Guide {
    float position;
    bool vertical; // true = vertical, false = horizontal
    glm::vec4 color;
};

struct CanvasComponent {
    std::unique_ptr<Blend2DRenderer> renderer;
    std::shared_ptr<Graphics> graphics;
    std::unique_ptr<Canvas> canvas;
    float zoom = 1.0f;
    glm::vec2 offset = glm::vec2(0.0f, 0.0f);
    bool showRulers = false;
    bool showGuides = false;
    std::vector<Guide> guides;

    CanvasComponent(int width, int height) {
        renderer = std::make_unique<Blend2DRenderer>();
        renderer->initialize(width, height);
        graphics = std::make_shared<Graphics>();
        graphics->setRenderer(renderer.get());
        canvas = std::make_unique<Canvas>(width, height, graphics);
    }
}; 