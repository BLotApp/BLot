#pragma once

#include "Window.h"
#include <functional>
#include <vector>
#include <string>

// Forward declarations
class ECSManager;

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
    void setCanvasEntities(const std::vector<std::pair<uint32_t, std::string>>& entities) { m_canvasEntities = entities; }
    void setActiveCanvasId(uint32_t id) { m_activeCanvasId = id; }
    void setCloseCanvasCallback(std::function<void(uint32_t)> callback) { m_closeCanvasCallback = callback; }
    void setSelectCanvasCallback(std::function<void(uint32_t)> callback) { m_selectCanvasCallback = callback; }

    // Window visibility management
    void setWindowVisibilityCallback(std::function<void(const std::string&, bool)> callback) { m_windowVisibilityCallback = callback; }
    void setGetWindowVisibilityCallback(std::function<bool(const std::string&)> callback) { m_getWindowVisibilityCallback = callback; }
    void setGetAllWindowsCallback(std::function<std::vector<std::string>()> callback) { m_getAllWindowsCallback = callback; }
    
    // Workspace management
    void setLoadWorkspaceCallback(std::function<void(const std::string&)> callback) { m_loadWorkspaceCallback = callback; }
    void setSaveWorkspaceCallback(std::function<void(const std::string&)> callback) { m_saveWorkspaceCallback = callback; }
    void setSaveWorkspaceAsCallback(std::function<void(const std::string&)> callback) { m_saveWorkspaceAsCallback = callback; }
    void setShowSaveWorkspaceDialogCallback(std::function<void()> callback) { m_showSaveWorkspaceDialogCallback = callback; }
    void setCurrentWorkspaceCallback(std::function<std::string()> callback) { m_getCurrentWorkspaceCallback = callback; }
    void setGetAvailableWorkspacesCallback(std::function<std::vector<std::pair<std::string, std::string>>()> callback) { m_getAvailableWorkspacesCallback = callback; }
    
    // Debug mode management
    void setDebugModeCallback(std::function<void(bool)> callback) { m_debugModeCallback = callback; }
    void setGetDebugModeCallback(std::function<bool()> callback) { m_getDebugModeCallback = callback; }

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
    std::vector<std::pair<uint32_t, std::string>> m_canvasEntities;
    uint32_t m_activeCanvasId = 0;
    std::function<void(uint32_t)> m_closeCanvasCallback;
    std::function<void(uint32_t)> m_selectCanvasCallback;
    
    // Window visibility management
    std::function<void(const std::string&, bool)> m_windowVisibilityCallback;
    std::function<bool(const std::string&)> m_getWindowVisibilityCallback;
    std::function<std::vector<std::string>()> m_getAllWindowsCallback;
    
    // Workspace management
    std::function<void(const std::string&)> m_loadWorkspaceCallback;
    std::function<void(const std::string&)> m_saveWorkspaceCallback;
    std::function<void(const std::string&)> m_saveWorkspaceAsCallback;
    std::function<void()> m_showSaveWorkspaceDialogCallback;
    std::function<std::string()> m_getCurrentWorkspaceCallback;
    std::function<std::vector<std::pair<std::string, std::string>>()> m_getAvailableWorkspacesCallback;
    
    // Debug mode management
    std::function<void(bool)> m_debugModeCallback;
    std::function<bool()> m_getDebugModeCallback;
};

} // namespace blot 