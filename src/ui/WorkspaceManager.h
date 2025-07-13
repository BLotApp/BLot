#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "WindowManager.h"

namespace blot {

struct WorkspaceConfig {
    std::string name;
    std::string description;
    std::vector<std::string> defaultWindows;
    std::map<std::string, bool> windowVisibility;
    std::map<std::string, std::pair<float, float>> windowPositions;
    std::map<std::string, std::pair<float, float>> windowSizes;
    std::string imguiLayout;
};

class WorkspaceManager {
public:
    WorkspaceManager();
    ~WorkspaceManager() = default;

    // Workspace management
    bool loadWorkspace(const std::string& workspaceName);
    bool saveWorkspace(const std::string& workspaceName);
    bool saveWorkspaceAs(const std::string& workspaceName);
    bool createWorkspace(const std::string& workspaceName, const WorkspaceConfig& config);
    bool deleteWorkspace(const std::string& workspaceName);
    
    // Workspace queries
    std::vector<std::string> getAvailableWorkspaces() const;
    std::vector<std::pair<std::string, std::string>> getAvailableWorkspacesWithNames() const;
    WorkspaceConfig getWorkspaceConfig(const std::string& workspaceName) const;
    std::string getCurrentWorkspace() const { return m_currentWorkspace; }
    
    // Window state management
    void setWindowVisibility(const std::string& windowName, bool visible);
    bool getWindowVisibility(const std::string& windowName) const;
    void setWindowPosition(const std::string& windowName, float x, float y);
    void setWindowSize(const std::string& windowName, float width, float height);
    std::pair<float, float> getWindowPosition(const std::string& windowName) const;
    std::pair<float, float> getWindowSize(const std::string& windowName) const;
    
    // ImGui layout management
    void saveCurrentImGuiLayout();
    void loadImGuiLayout(const std::string& layoutData);
    std::string getCurrentImGuiLayout() const;
    
    // UI state capture
    WorkspaceConfig captureCurrentUIState(const std::string& workspaceName, WindowManager* windowManager);
    
    // Utility methods
    void updateMainIniFile();
    void setWindowManager(WindowManager* windowManager) { m_windowManager = windowManager; }

private:
    std::string m_workspaceDir;
    std::string m_mainIniPath;
    std::map<std::string, WorkspaceConfig> m_workspaces;
    std::string m_currentWorkspace;
    WindowManager* m_windowManager = nullptr;

    // Helper methods
    void ensureWorkspaceDirectory();
    void createDefaultWorkspaces();
    void loadExistingWorkspaces();
    std::string getWorkspaceConfigPath(const std::string& workspaceName) const;
    bool loadWorkspaceConfig(const std::string& workspaceName);
    bool saveWorkspaceConfig(const std::string& workspaceName);
    WorkspaceConfig createWorkspaceFromCurrentState(const std::string& workspaceName);
    
    // Helper function for loading optional JSON objects
    template<typename T>
    T loadOptionalJsonObject(const nlohmann::json& j, const std::string& key, const T& defaultValue = T{}) {
        return j.value(key, defaultValue);
    }
};

} // namespace blot 