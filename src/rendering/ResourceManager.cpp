#include "rendering/ResourceManager.h"

ResourceManager::ResourceManager() {
}

ResourceManager::~ResourceManager() {
    cleanup();
}

std::shared_ptr<Blend2DRenderer> ResourceManager::getRenderer(entt::entity entity) {
    auto it = m_renderers.find(entity);
    if (it != m_renderers.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<Blend2DRenderer> ResourceManager::createRenderer(entt::entity entity, int width, int height) {
    auto renderer = std::make_shared<Blend2DRenderer>();
    if (renderer->initialize(width, height)) {
        m_renderers[entity] = renderer;
        return renderer;
    }
    return nullptr;
}

void ResourceManager::destroyRenderer(entt::entity entity) {
    auto it = m_renderers.find(entity);
    if (it != m_renderers.end()) {
        m_renderers.erase(it);
    }
}

void ResourceManager::cleanup() {
    m_renderers.clear();
    m_canvases.clear();
    m_graphics.clear();
}

// Canvas management
std::unique_ptr<Canvas>* ResourceManager::getCanvas(entt::entity entity) {
    auto it = m_canvases.find(entity);
    if (it != m_canvases.end()) {
        return &it->second;
    }
    return nullptr;
}

void ResourceManager::addCanvas(entt::entity entity, std::unique_ptr<Canvas> canvas) {
    m_canvases[entity] = std::move(canvas);
}

void ResourceManager::removeCanvas(entt::entity entity) {
    auto it = m_canvases.find(entity);
    if (it != m_canvases.end()) {
        m_canvases.erase(it);
    }
}

// Graphics management
std::shared_ptr<Graphics> ResourceManager::getGraphics(entt::entity entity) {
    auto it = m_graphics.find(entity);
    if (it != m_graphics.end()) {
        return it->second;
    }
    return nullptr;
}

void ResourceManager::addGraphics(entt::entity entity, std::shared_ptr<Graphics> graphics) {
    m_graphics[entity] = graphics;
}

void ResourceManager::removeGraphics(entt::entity entity) {
    auto it = m_graphics.find(entity);
    if (it != m_graphics.end()) {
        m_graphics.erase(it);
    }
} 