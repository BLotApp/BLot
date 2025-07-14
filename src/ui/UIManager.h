#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>

// Forward declarations
struct GLFWwindow;
#include "windows/SaveWorkspaceDialog.h"
class BlotApp;
#include "windows/MainMenuBar.h"

#include "WindowManager.h"
#include "WorkspaceManager.h"
#include "windows/DebugPanel.h"
#include "windows/InfoWindow.h"
#include "windows/ThemePanel.h"
#include "windows/TerminalWindow.h"
#include "windows/LogWindow.h"
#include "ui/TextRenderer.h"
#include "ui/ImGuiRenderer.h"
#include <GLFW/glfw3.h>
#include "ShortcutManager.h"

// Forward declarations
namespace blot {
    class SaveWorkspaceDialog;
}

namespace blot {

class UIManager {
public:
    UIManager(GLFWwindow* window);
    ~UIManager();

    // Main UI operations
    void update();
    void handleInput();
    
    // ImGui initialization and shutdown
    void initImGui();
    void shutdownImGui();
    
    // Window management
    void setupDockspace();
    void renderAllWindows();
    
    // Window visibility management
    void setWindowVisibility(const std::string& windowName, bool visible);
    void setWindowVisibilityAll(bool visible);
    bool getWindowVisibility(const std::string& windowName) const;
    std::vector<std::string> getAllWindowNames() const;
    
    // Window callback setup
    void setupWindowCallbacks(BlotApp* app);
    
    // Workspace management
    bool loadWorkspace(const std::string& workspaceName);
    bool saveWorkspace(const std::string& workspaceName);
    bool saveWorkspaceAs(const std::string& workspaceName);
    std::string getCurrentWorkspace() const;
    std::vector<std::pair<std::string, std::string>> getAvailableWorkspaces() const;
    std::vector<std::string> getAllWorkspaceNames() const;
    void saveCurrentImGuiLayout();
    
    // Getters for external access
    WindowManager* getWindowManager() { return m_windowManager.get(); }
    WorkspaceManager* getWorkspaceManager() { return m_workspaceManager.get(); }
    ShortcutManager& getShortcutManager() { return m_shortcutManager; }
   
    // Templated window getters for type-safe access
    template<typename T>
    std::shared_ptr<T> getWindowAs(const std::string& name) {
        return m_windowManager->getWindowAs<T>(name);
    }
    
    template<typename T>
    std::shared_ptr<T> getFocusedWindowAs() {
        return m_windowManager->getFocusedWindowAs<T>();
    }
    
    // Text renderer access
    TextRenderer* getTextRenderer() { return m_textRenderer.get(); }
    ImGuiRenderer* getImGuiRenderer() { return m_imguiRenderer.get(); }
    
    // Debug mode control
    void setDebugMode(bool enabled) { m_debugMode = enabled; }
    bool getDebugMode() const { return m_debugMode; }
    
    // Save workspace dialog access
    SaveWorkspaceDialog* getSaveWorkspaceDialog() { return m_saveWorkspaceDialog.get(); }

    // Global ImGui theme for the entire UI
    enum class ImGuiTheme { Dark, Light, Classic, Corporate, Dracula };
    ImGuiTheme m_currentTheme = ImGuiTheme::Light;
    void setImGuiTheme(ImGuiTheme theme);
    ImGuiTheme getImGuiTheme() const { return m_currentTheme; }
    
    // Theme file management
    std::string m_lastThemePath = "themes/default.json";
    void saveCurrentTheme(const std::string& path);
    void loadTheme(const std::string& path);

private:
    // GLFW window reference
    GLFWwindow* m_window;
    
    // Core window manager
    std::unique_ptr<WindowManager> m_windowManager;
    
    // Workspace manager
    std::unique_ptr<WorkspaceManager> m_workspaceManager;
    
    // Save workspace dialog (still managed as unique_ptr, but registered with WindowManager)
    std::unique_ptr<SaveWorkspaceDialog> m_saveWorkspaceDialog;

    // Hide all windows except menubar flag
    bool m_bHideWindows = false;

    // Shortcut manager
    ShortcutManager m_shortcutManager;
    
    // ImGui with enhanced text rendering
    std::unique_ptr<TextRenderer> m_textRenderer;
    std::unique_ptr<ImGuiRenderer> m_imguiRenderer;
    
    // Debug mode
    bool m_debugMode = false;
    
    // Setup methods
    void setupWindows();
    void configureWindowSettings();
    std::unique_ptr<MainMenuBar> m_mainMenuBar;
};

} // namespace blot 