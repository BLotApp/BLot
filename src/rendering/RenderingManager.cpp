#include "rendering/RenderingManager.h"
#include "rendering/RendererRegistry.h"
#include "core/ISettings.h"

namespace blot {

RenderingManager::RenderingManager() {
}

RenderingManager::~RenderingManager() {
    cleanup();
}

std::shared_ptr<IRenderer> RenderingManager::getRenderer(entt::entity entity) {
    auto it = m_renderers.find(entity);
    if (it != m_renderers.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<IRenderer> RenderingManager::createRenderer(entt::entity entity, RendererType type, int width, int height) {
    auto uniqueRenderer = RendererRegistry::instance().create(type);
    if (uniqueRenderer && uniqueRenderer->initialize(width, height)) {
        std::shared_ptr<IRenderer> renderer = std::move(uniqueRenderer);
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

void RenderingManager::setMainRenderer(std::shared_ptr<IRenderer> renderer) {
    m_mainRenderer = renderer;
}

std::shared_ptr<IRenderer> RenderingManager::getMainRenderer() const {
    return m_mainRenderer;
}

blot::json RenderingManager::getSettings() const {
    blot::json j;
    // Save main renderer type if available
    if (m_mainRenderer) {
        j["mainRendererType"] = static_cast<int>(m_mainRenderer->getType());
    }
    // Save entity associations
    j["renderers"] = blot::json::array();
    for (const auto& [entity, renderer] : m_renderers) {
        if (renderer) {
            blot::json rj;
            rj["entity"] = static_cast<uint32_t>(entity);
            rj["type"] = static_cast<int>(renderer->getType());
            j["renderers"].push_back(rj);
        }
    }
    j["canvases"] = blot::json::array();
    for (const auto& [entity, canvas] : m_canvases) {
        blot::json cj;
        cj["entity"] = static_cast<uint32_t>(entity);
        if (canvas) cj["settings"] = canvas->getSettings();
        j["canvases"].push_back(cj);
    }
    // Optionally, save graphics associations
    // ...
    return j;
}

void RenderingManager::setSettings(const blot::json& settings) {
    // Restore main renderer type if needed
    // (You may want to recreate the renderer here)
    // Restore entity associations
    if (settings.contains("renderers")) {
        m_renderers.clear();
        for (const auto& rj : settings["renderers"]) {
            entt::entity entity = static_cast<entt::entity>(rj["entity"].get<uint32_t>());
            RendererType type = static_cast<RendererType>(rj["type"].get<int>());
            auto renderer = RendererRegistry::instance().create(type);
            if (renderer) m_renderers[entity] = std::move(renderer);
        }
    }
    if (settings.contains("canvases")) {
        m_canvases.clear();
        for (const auto& cj : settings["canvases"]) {
            entt::entity entity = static_cast<entt::entity>(cj["entity"].get<uint32_t>());
            if (cj.contains("settings")) {
                auto canvas = std::make_unique<Canvas>(CanvasSettings());
                canvas->setSettings(cj["settings"]);
                m_canvases[entity] = std::move(canvas);
            }
        }
    }
    // Optionally, restore graphics associations
    // ...
}

} 