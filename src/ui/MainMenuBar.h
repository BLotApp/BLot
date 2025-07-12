#pragma once

#include "Window.h"
#include <functional>

namespace blot {

class MainMenuBar : public Window {
public:
    MainMenuBar(const std::string& title, Window::Flags flags = Window::Flags::None);
    ~MainMenuBar() = default;

    // Menu callbacks
    void setQuitCallback(std::function<void()> callback) { m_quitCallback = callback; }
    void setNewSketchCallback(std::function<void()> callback) { m_newSketchCallback = callback; }
    void setOpenSketchCallback(std::function<void()> callback) { m_openSketchCallback = callback; }
    void setSaveSketchCallback(std::function<void()> callback) { m_saveSketchCallback = callback; }
    void setRunSketchCallback(std::function<void()> callback) { m_runSketchCallback = callback; }
    void setStopSketchCallback(std::function<void()> callback) { m_stopSketchCallback = callback; }
    void setAddonManagerCallback(std::function<void()> callback) { m_addonManagerCallback = callback; }
    void setReloadAddonsCallback(std::function<void()> callback) { m_reloadAddonsCallback = callback; }
    void setThemeEditorCallback(std::function<void()> callback) { m_themeEditorCallback = callback; }
    void setNewCanvasCallback(std::function<void()> callback) { m_newCanvasCallback = callback; }
    void setSaveCanvasCallback(std::function<void()> callback) { m_saveCanvasCallback = callback; }
    void setOpenMarkdownCallback(std::function<void()> callback) { m_openMarkdownCallback = callback; }
    void setSaveMarkdownCallback(std::function<void()> callback) { m_saveMarkdownCallback = callback; }
    void setImPlotDemoCallback(std::function<void()> callback) { m_imPlotDemoCallback = callback; }
    void setImGuiMarkdownDemoCallback(std::function<void()> callback) { m_imGuiMarkdownDemoCallback = callback; }
    void setMarkdownEditorCallback(std::function<void()> callback) { m_markdownEditorCallback = callback; }
    
    // Renderer callbacks
    void setSwitchRendererCallback(std::function<void(int)> callback) { m_switchRendererCallback = callback; }
    void setCurrentRendererType(int type) { m_currentRendererType = type; }
    
    // Theme callbacks
    void setSwitchThemeCallback(std::function<void(int)> callback) { m_switchThemeCallback = callback; }
    void setCurrentTheme(int theme) { m_currentTheme = theme; }
    
    // Canvas management
    void setCanvasEntities(const std::vector<std::pair<entt::entity, std::string>>& entities) { m_canvasEntities = entities; }
    void setActiveCanvasId(entt::entity id) { m_activeCanvasId = id; }
    void setCloseCanvasCallback(std::function<void(entt::entity)> callback) { m_closeCanvasCallback = callback; }
    void setSelectCanvasCallback(std::function<void(entt::entity)> callback) { m_selectCanvasCallback = callback; }

protected:
    void render() override;

private:
    // Menu callbacks
    std::function<void()> m_quitCallback;
    std::function<void()> m_newSketchCallback;
    std::function<void()> m_openSketchCallback;
    std::function<void()> m_saveSketchCallback;
    std::function<void()> m_runSketchCallback;
    std::function<void()> m_stopSketchCallback;
    std::function<void()> m_addonManagerCallback;
    std::function<void()> m_reloadAddonsCallback;
    std::function<void()> m_themeEditorCallback;
    std::function<void()> m_newCanvasCallback;
    std::function<void()> m_saveCanvasCallback;
    std::function<void()> m_openMarkdownCallback;
    std::function<void()> m_saveMarkdownCallback;
    std::function<void()> m_imPlotDemoCallback;
    std::function<void()> m_imGuiMarkdownDemoCallback;
    std::function<void()> m_markdownEditorCallback;
    
    // Renderer callbacks
    std::function<void(int)> m_switchRendererCallback;
    int m_currentRendererType = 0;
    
    // Theme callbacks
    std::function<void(int)> m_switchThemeCallback;
    int m_currentTheme = 0;
    
    // Canvas management
    std::vector<std::pair<entt::entity, std::string>> m_canvasEntities;
    entt::entity m_activeCanvasId = entt::null;
    std::function<void(entt::entity)> m_closeCanvasCallback;
    std::function<void(entt::entity)> m_selectCanvasCallback;
};

} // namespace blot 