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
} 