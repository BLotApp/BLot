#pragma once

#include "WindowManager.h"
#include "DebugPanel.h"
#include "InfoWindow.h"
#include "ThemePanel.h"
#include "TerminalWindow.h"
#include "LogWindow.h"
#include "ui/TextRenderer.h"
#include "ui/ImGuiRenderer.h"
#include <memory>
#include <GLFW/glfw3.h>

namespace blot {

class UIManager {
public:
    UIManager(GLFWwindow* window);
    ~UIManager() = default;

    // Main UI operations
    void render();
    void handleInput();
    void update();
    
    // ImGui initialization
    void initImGui();
    
    // Window management
    void setupDockspace();
    void renderAllWindows();
    
    // Getters for external access
    WindowManager* getWindowManager() { return m_windowManager.get(); }
    DebugPanel* getDebugPanel() { return m_debugPanel.get(); }
    InfoWindow* getInfoWindow() { return m_infoWindow.get(); }
    ThemePanel* getThemePanel() { return m_themePanel.get(); }
    TerminalWindow* getTerminalWindow() { return m_terminalWindow.get(); }
    LogWindow* getLogWindow() { return m_logWindow.get(); }
    
    // Text renderer access
    TextRenderer* getTextRenderer() { return m_textRenderer.get(); }
    ImGuiRenderer* getImGuiRenderer() { return m_imguiRenderer.get(); }

private:
    // GLFW window reference
    GLFWwindow* m_window;
    
    // Core window manager
    std::unique_ptr<WindowManager> m_windowManager;
    
    // UI Panels
    std::unique_ptr<DebugPanel> m_debugPanel;
    std::unique_ptr<InfoWindow> m_infoWindow;
    std::unique_ptr<ThemePanel> m_themePanel;
    
    // New windows
    std::unique_ptr<TerminalWindow> m_terminalWindow;
    std::unique_ptr<LogWindow> m_logWindow;
    
    // ImGui with enhanced text rendering
    std::unique_ptr<TextRenderer> m_textRenderer;
    std::unique_ptr<ImGuiRenderer> m_imguiRenderer;
    
    // Setup methods
    void setupWindows();
    void configureWindowSettings();
};

} // namespace blot 