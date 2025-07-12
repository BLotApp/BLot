#pragma once

#include <memory>
#include <unordered_map>
#include <entt/entt.hpp>
#include "rendering/Blend2DRenderer.h"
#include "rendering/Renderer.h"
#include "canvas/Canvas.h"
#include "rendering/Graphics.h"

class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();
    
    // Renderer management
    std::shared_ptr<Blend2DRenderer> getRenderer(entt::entity entity);
    std::shared_ptr<Blend2DRenderer> createRenderer(entt::entity entity, int width, int height);
    void destroyRenderer(entt::entity entity);
    
    // Canvas management
    std::unique_ptr<Canvas>* getCanvas(entt::entity entity);
    void addCanvas(entt::entity entity, std::unique_ptr<Canvas> canvas);
    void removeCanvas(entt::entity entity);
    std::unordered_map<entt::entity, std::unique_ptr<Canvas>>& getCanvases() { return m_canvases; }
    
    // Graphics management
    std::shared_ptr<Graphics> getGraphics(entt::entity entity);
    void addGraphics(entt::entity entity, std::shared_ptr<Graphics> graphics);
    void removeGraphics(entt::entity entity);
    
    // Resource cleanup
    void cleanup();
    
private:
    std::unordered_map<entt::entity, std::shared_ptr<Blend2DRenderer>> m_renderers;
    std::unordered_map<entt::entity, std::unique_ptr<Canvas>> m_canvases;
    std::unordered_map<entt::entity, std::shared_ptr<Graphics>> m_graphics;
}; 