#pragma once

#include <memory>
#include <unordered_map>
#include <entt/entt.hpp>
#include "rendering/Blend2DRenderer.h"
#include "rendering/Renderer.h"

class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();
    
    // Renderer management
    std::shared_ptr<Blend2DRenderer> getRenderer(entt::entity entity);
    std::shared_ptr<Blend2DRenderer> createRenderer(entt::entity entity, int width, int height);
    void destroyRenderer(entt::entity entity);
    
    // Resource cleanup
    void cleanup();
    
private:
    std::unordered_map<entt::entity, std::shared_ptr<Blend2DRenderer>> m_renderers;
}; 