#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <deque>

// Forward declarations
struct GLFWwindow;
#include "windows/SaveWorkspaceDialog.h"
class BlotApp;
#include "windows/MainMenuBar.h"

#include "WindowManager.h"
#include "windows/DebugPanel.h"
#include "windows/InfoWindow.h"
#include "windows/ThemePanel.h"
#include "windows/TerminalWindow.h"
#include "windows/LogWindow.h"
#include "ui/TextRenderer.h"
#include "ui/ImGuiRenderer.h"
#include <GLFW/glfw3.h>
#include "ShortcutManager.h"
#include "../third_party/IconFontCppHeaders/IconsFontAwesome5.h"

// Forward declarations
namespace blot {
    class SaveWorkspaceDialog;
}

namespace blot {

// Notification types
enum class NotificationType { Info, Success, Warning, Error };

struct Notification {
    std::string message;
    NotificationType type;
    float timeRemaining;
};

struct Modal {
    std::string title;
    std::string message;
    NotificationType type;
    std::function<void()> onOk;
    bool open = true;
};

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
    
    // Workspace management (now via WindowManager)
    bool loadWorkspace(const std::string& workspaceName);
    bool saveWorkspace(const std::string& workspaceName);
    bool saveWorkspaceAs(const std::string& workspaceName);
    std::string getCurrentWorkspace() const;
    std::vector<std::pair<std::string, std::string>> getAvailableWorkspaces() const;
    std::vector<std::string> getAllWorkspaceNames() const;
    void saveCurrentImGuiLayout();
    
    // Getters for external access
    WindowManager* getWindowManager() { return m_windowManager.get(); }
    ShortcutManager& getShortcutManager() { return m_shortcutManager; }
    BlotApp* getBlotApp() const { return m_blotApp; }
   
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

    void setupWindows(BlotApp* app);

    MainMenuBar* getMainMenuBar() { return m_mainMenuBar.get(); }

    // Notification/Popup API
    void showNotification(const std::string& message, NotificationType type = NotificationType::Info, float duration = 3.0f);
    void showModal(const std::string& title, const std::string& message, NotificationType type = NotificationType::Info, std::function<void()> onOk = nullptr);

    void setBlotApp(BlotApp* app) { m_blotApp = app; }

private:
    // GLFW window reference
    GLFWwindow* m_window;
    
    // Core window manager
    std::unique_ptr<WindowManager> m_windowManager;
    
    // Save workspace dialog (still managed as unique_ptr, but registered with WindowManager)
    std::unique_ptr<SaveWorkspaceDialog> m_saveWorkspaceDialog;

    // Hide all windows except menubar flag
    bool m_bHideWindows = false;
    bool m_bHideMainMenuBar = false;

    // Shortcut manager
    ShortcutManager m_shortcutManager;
    
    // ImGui with enhanced text rendering
    std::unique_ptr<TextRenderer> m_textRenderer;
    std::unique_ptr<ImGuiRenderer> m_imguiRenderer;
    
    // Setup methods
    void configureWindowSettings();
    std::unique_ptr<MainMenuBar> m_mainMenuBar;

    std::deque<Notification> m_notifications;
    std::deque<Modal> m_modals;

    BlotApp* m_blotApp = nullptr;
};

} // namespace blot 