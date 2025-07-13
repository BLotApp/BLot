#include "rendering/RenderingManager.h"

RenderingManager::RenderingManager() {
}

RenderingManager::~RenderingManager() {
    cleanup();
}

std::shared_ptr<Blend2DRenderer> RenderingManager::getRenderer(entt::entity entity) {
    auto it = m_renderers.find(entity);
    if (it != m_renderers.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<Blend2DRenderer> RenderingManager::createRenderer(entt::entity entity, int width, int height) {
    auto renderer = std::make_shared<Blend2DRenderer>();
    if (renderer->initialize(width, height)) {
        m_renderers[entity] = renderer;
        return renderer;
    }
    return nullptr;
}

void RenderingManager::destroyRenderer(entt::entity entity) {
    auto it = m_renderers.find(entity);
    if (it != m_renderers.end()) {
        m_renderers.erase(it);
    }
}

void RenderingManager::cleanup() {
    m_renderers.clear();
    m_canvases.clear();
    m_graphics.clear();
}

// Canvas management
std::unique_ptr<Canvas>* RenderingManager::getCanvas(entt::entity entity) {
    auto it = m_canvases.find(entity);
    if (it != m_canvases.end()) {
        return &it->second;
    }
    return nullptr;
}

void RenderingManager::addCanvas(entt::entity entity, std::unique_ptr<Canvas> canvas) {
    m_canvases[entity] = std::move(canvas);
}

void RenderingManager::removeCanvas(entt::entity entity) {
    auto it = m_canvases.find(entity);
    if (it != m_canvases.end()) {
        m_canvases.erase(it);
    }
}

// Graphics management
std::shared_ptr<Graphics> RenderingManager::getGraphics(entt::entity entity) {
    auto it = m_graphics.find(entity);
    if (it != m_graphics.end()) {
        return it->second;
    }
    return nullptr;
}

void RenderingManager::addGraphics(entt::entity entity, std::shared_ptr<Graphics> graphics) {
    m_graphics[entity] = graphics;
}

void RenderingManager::removeGraphics(entt::entity entity) {
    auto it = m_graphics.find(entity);
    if (it != m_graphics.end()) {
        m_graphics.erase(it);
    }
} 