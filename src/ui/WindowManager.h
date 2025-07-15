#pragma once

#include <memory>
#include <vector>
#include <string>
#include <entt/entt.hpp>
#include "windows/Window.h"

namespace blot {

// Window Components
struct WindowComponent {
    std::shared_ptr<Window> window;
    std::string name;
    bool isVisible = true;
    bool isFocused = false;
    int zOrder = 0;
};

struct WindowTransformComponent {
    ImVec2 position = ImVec2(0, 0);
    ImVec2 size = ImVec2(400, 300);
    ImVec2 minSize = ImVec2(100, 100);
    ImVec2 maxSize = ImVec2(FLT_MAX, FLT_MAX);
};

struct WindowStyleComponent {
    float alpha = 1.0f;
    int flags = 0;
    ImVec4 backgroundColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
};

struct WindowInputComponent {
    bool handleMouse = true;
    bool handleKeyboard = true;
    bool closeOnEscape = true;
};

struct WindowSettingsComponent {
    bool showInMenu = true;
    bool showByDefault = true;
    bool canBeHidden = true;
    bool canBeMoved = true;
    bool canBeResized = true;
    bool canBeFocused = true;
    std::string menuPath = ""; // e.g., "Windows/Canvas"
    std::string category = "General"; // e.g., "Tools", "Debug", "Main"
};

// WorkspaceConfig struct (moved from WorkspaceManager)
struct WorkspaceConfig {
    std::string name;
    std::string description;
    std::map<std::string, bool> windowVisibility;
    std::string imguiLayout;
};

class WindowManager {
public:
    WindowManager();
    ~WindowManager();

    // ECS-style window management
    entt::entity createWindow(const std::string& name, std::shared_ptr<Window> window);
    void destroyWindow(entt::entity windowEntity);
    void destroyWindow(const std::string& windowName);
    
    // Window queries
    entt::entity getWindowEntity(const std::string& name);
    std::shared_ptr<Window> getWindow(const std::string& name);
    std::shared_ptr<Window> getFocusedWindow();
    entt::entity getFocusedWindowEntity();
    std::vector<std::string> getAllWindowNames() const;
    std::vector<std::pair<std::string, std::string>> getAllWindowsWithDisplayNames() const;
    
    // Templated window getters for type-safe access
    template<typename T>
    std::shared_ptr<T> getWindowAs(const std::string& name) {
        auto window = getWindow(name);
        return std::dynamic_pointer_cast<T>(window);
    }
    
    template<typename T>
    std::shared_ptr<T> getFocusedWindowAs() {
        auto window = getFocusedWindow();
        return std::dynamic_pointer_cast<T>(window);
    }
    
    // Window operations
    void showWindow(const std::string& name);
    void hideWindow(const std::string& name);
    void closeWindow(const std::string& name);
    void focusWindow(const std::string& name);
    void closeFocusedWindow();
    void closeAllWindows();
    
    // Window visibility management
    bool isWindowVisible(const std::string& name);
    void setWindowVisible(const std::string& name, bool visible);
    void toggleWindow(const std::string& name);
    void showAllWindows();
    void hideAllWindows(const std::vector<std::string>& except = {"MainMenuBar"});
    void setMainMenuBar(bool visible);
    
    // Window settings management
    void setWindowSettings(const std::string& name, const WindowSettingsComponent& settings);
    WindowSettingsComponent getWindowSettings(const std::string& name);
    std::vector<std::string> getVisibleWindows();
    std::vector<std::string> getHiddenWindows();
    std::vector<std::string> getWindowsByCategory(const std::string& category);
    
    // Menu integration
    void renderWindowMenu();
    std::vector<std::string> getMenuWindows();
    
    // Rendering and input
    void renderAllWindows();
    void handleInput();
    void update();
    
    // Workspace management (moved from WorkspaceManager)
    bool loadWorkspace(const std::string& workspaceName);
    bool saveWorkspace(const std::string& workspaceName);
    bool saveWorkspaceAs(const std::string& workspaceName);
    bool createWorkspace(const std::string& workspaceName, const WorkspaceConfig& config);
    bool deleteWorkspace(const std::string& workspaceName);
    std::vector<std::string> getAvailableWorkspaces() const;
    std::vector<std::pair<std::string, std::string>> getAvailableWorkspacesWithNames() const;
    WorkspaceConfig getWorkspaceConfig(const std::string& workspaceName) const;
    std::string getCurrentWorkspace() const { return m_currentWorkspace; }
    void setWindowVisibility(const std::string& windowName, bool visible);
    bool getWindowVisibility(const std::string& windowName) const;
    void setWindowPosition(const std::string& windowName, float x, float y);
    void setWindowSize(const std::string& windowName, float width, float height);
    std::pair<float, float> getWindowPosition(const std::string& windowName) const;
    std::pair<float, float> getWindowSize(const std::string& windowName) const;
    void saveCurrentImGuiLayout();
    void loadImGuiLayout(const std::string& layoutData);
    std::string getCurrentImGuiLayout() const;
    
    // Registry access
    entt::registry& getRegistry() { return m_registry; }
    const entt::registry& getRegistry() const { return m_registry; }

private:
    entt::registry m_registry;
    entt::entity m_focusedWindowEntity = entt::null;
    
    void updateFocus();
    void handleEscapeKey();
    void sortWindowsByZOrder();

    // Workspace-related members (paths set via AppPaths utility)
    std::string m_workspaceDir;
    std::string m_mainIniPath;
    std::map<std::string, WorkspaceConfig> m_workspaces;
    std::string m_currentWorkspace;

    // Helper methods for workspace management
    void ensureWorkspaceDirectory();
    void createDefaultWorkspaces();
    void loadExistingWorkspaces();
    std::string getWorkspaceConfigPath(const std::string& workspaceName) const;
    bool loadWorkspaceConfig(const std::string& workspaceName);
    bool saveWorkspaceConfig(const std::string& workspaceName);
    WorkspaceConfig captureCurrentUIState(const std::string& workspaceName);
    WorkspaceConfig createWorkspaceFromCurrentState(const std::string& workspaceName);
    void updateMainIniFile();
};

} // namespace blot 