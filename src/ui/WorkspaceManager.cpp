#include "WorkspaceManager.h"
#include "imgui.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <ctime>
#include <sstream>

namespace blot {

WorkspaceManager::WorkspaceManager() {
    m_workspaceDir = "workspaces";
    m_mainIniPath = "imgui.ini"; // Use the main ImGui .ini file
    ensureWorkspaceDirectory();
    createDefaultWorkspaces();
    loadExistingWorkspaces();
}

bool WorkspaceManager::loadWorkspace(const std::string& workspaceName) {
    std::cout << "Loading workspace: " << workspaceName << std::endl;
    
    if (m_workspaces.find(workspaceName) == m_workspaces.end()) {
        std::cerr << "Workspace '" << workspaceName << "' not found" << std::endl;
        return false;
    }
    
    // Load workspace configuration
    std::cout << "Loading workspace configuration..." << std::endl;
    if (!loadWorkspaceConfig(workspaceName)) {
        std::cerr << "Failed to load workspace config for '" << workspaceName << "'" << std::endl;
        return false;
    }
    
    // Apply window visibility from workspace
    const auto& config = m_workspaces[workspaceName];
    if (m_windowManager) {
        std::cout << "Applying window visibility from workspace..." << std::endl;
        for (const auto& [windowName, isVisible] : config.windowVisibility) {
            std::cout << "  Setting " << windowName << " to " << (isVisible ? "visible" : "hidden") << std::endl;
            m_windowManager->setWindowVisible(windowName, isVisible);
        }
    } else {
        std::cout << "WindowManager not available, skipping window visibility" << std::endl;
    }
    
    // Load ImGui layout from the workspace
    if (!config.imguiLayout.empty()) {
        std::cout << "Loading ImGui layout from workspace..." << std::endl;
        // Temporarily disable ImGui layout loading to debug crash
        // loadImGuiLayout(config.imguiLayout);
        std::cout << "ImGui layout loading temporarily disabled for debugging" << std::endl;
    } else {
        std::cout << "No ImGui layout found in workspace, using default" << std::endl;
    }
    
    m_currentWorkspace = workspaceName;
    std::cout << "Loaded workspace: " << workspaceName << std::endl;
    return true;
}

bool WorkspaceManager::saveWorkspace(const std::string& workspaceName) {
    // Capture current UI state instead of relying on ImGui .ini
    WorkspaceConfig currentState = captureCurrentUIState(workspaceName, m_windowManager);
    
    // Update the workspace with current state
    m_workspaces[workspaceName] = currentState;
    
    // Save workspace configuration
    if (!saveWorkspaceConfig(workspaceName)) {
        std::cerr << "Failed to save workspace config for '" << workspaceName << "'" << std::endl;
        return false;
    }
    
    std::cout << "Saved workspace: " << workspaceName << std::endl;
    return true;
}

bool WorkspaceManager::saveWorkspaceAs(const std::string& workspaceName) {
    // Create a new workspace from current state
    WorkspaceConfig newConfig = captureCurrentUIState(workspaceName, m_windowManager);
    
    // Add the new workspace to our collection
    m_workspaces[workspaceName] = newConfig;
    
    // Save the workspace
    return saveWorkspace(workspaceName);
}

WorkspaceConfig WorkspaceManager::captureCurrentUIState(const std::string& workspaceName, WindowManager* windowManager) {
    std::cout << "Capturing current UI state for workspace: " << workspaceName << std::endl;
    
    WorkspaceConfig config;
    config.name = workspaceName;
    config.description = "Custom workspace created on " + std::to_string(std::time(nullptr));
    
    // Capture current window visibility from WindowManager
    if (windowManager) {
        std::cout << "WindowManager available, capturing window states..." << std::endl;
        std::vector<std::string> allWindows = windowManager->getAllWindowNames();
        std::cout << "Found " << allWindows.size() << " windows:" << std::endl;
        
        for (const auto& windowName : allWindows) {
            bool isVisible = windowManager->isWindowVisible(windowName);
            config.windowVisibility[windowName] = isVisible;
            std::cout << "  - " << windowName << ": " << (isVisible ? "visible" : "hidden") << std::endl;
        }
        
        // Set default windows to all currently visible windows
        std::vector<std::string> visibleWindows = windowManager->getVisibleWindows();
        config.defaultWindows = visibleWindows;
        std::cout << "Default windows (visible): " << visibleWindows.size() << std::endl;
    } else {
        std::cout << "WindowManager not available, using fallback defaults" << std::endl;
        // Fallback to default windows if no WindowManager available
        config.defaultWindows = {"CodeEditor", "Canvas", "Toolbar", "Info", "Properties"};
        config.windowVisibility = {
            {"CodeEditor", true},
            {"Canvas", true},
            {"Toolbar", true},
            {"Info", true},
            {"Properties", true},
            {"Stroke", true},
            {"AddonManager", false},
            {"NodeEditor", false},
            {"ThemeEditor", false},
            {"Texture", false}
        };
    }
    
    // Capture current ImGui layout
    config.imguiLayout = this->getCurrentImGuiLayout();
    std::cout << "ImGui layout captured: " << (config.imguiLayout.empty() ? "empty" : "non-empty") << std::endl;
    
    std::cout << "UI state capture complete" << std::endl;
    return config;
}

WorkspaceConfig WorkspaceManager::createWorkspaceFromCurrentState(const std::string& workspaceName) {
    return captureCurrentUIState(workspaceName, m_windowManager);
}

bool WorkspaceManager::createWorkspace(const std::string& workspaceName, const WorkspaceConfig& config) {
    if (m_workspaces.find(workspaceName) != m_workspaces.end()) {
        std::cerr << "Workspace '" << workspaceName << "' already exists" << std::endl;
        return false;
    }
    
    m_workspaces[workspaceName] = config;
    return saveWorkspaceConfig(workspaceName);
}

bool WorkspaceManager::deleteWorkspace(const std::string& workspaceName) {
    if (m_workspaces.find(workspaceName) == m_workspaces.end()) {
        return false;
    }
    
    // Remove workspace file
    std::filesystem::remove(getWorkspaceConfigPath(workspaceName));
    
    m_workspaces.erase(workspaceName);
    return true;
}

std::vector<std::string> WorkspaceManager::getAvailableWorkspaces() const {
    std::vector<std::string> workspaces;
    for (const auto& [name, config] : m_workspaces) {
        workspaces.push_back(name);
    }
    return workspaces;
}

std::vector<std::pair<std::string, std::string>> WorkspaceManager::getAvailableWorkspacesWithNames() const {
    std::vector<std::pair<std::string, std::string>> workspaces;
    for (const auto& [name, config] : m_workspaces) {
        workspaces.push_back({name, config.name});
    }
    return workspaces;
}

WorkspaceConfig WorkspaceManager::getWorkspaceConfig(const std::string& workspaceName) const {
    auto it = m_workspaces.find(workspaceName);
    if (it != m_workspaces.end()) {
        return it->second;
    }
    return WorkspaceConfig{};
}

void WorkspaceManager::setWindowVisibility(const std::string& windowName, bool visible) {
    if (m_currentWorkspace.empty()) return;
    
    auto& config = m_workspaces[m_currentWorkspace];
    config.windowVisibility[windowName] = visible;
}

bool WorkspaceManager::getWindowVisibility(const std::string& windowName) const {
    if (m_currentWorkspace.empty()) return true;
    
    auto it = m_workspaces.find(m_currentWorkspace);
    if (it == m_workspaces.end()) return true;
    
    auto visibilityIt = it->second.windowVisibility.find(windowName);
    return visibilityIt != it->second.windowVisibility.end() ? visibilityIt->second : true;
}

void WorkspaceManager::setWindowPosition(const std::string& windowName, float x, float y) {
    if (m_currentWorkspace.empty()) return;
    
    auto& config = m_workspaces[m_currentWorkspace];
    config.windowPositions[windowName] = {x, y};
}

void WorkspaceManager::setWindowSize(const std::string& windowName, float width, float height) {
    if (m_currentWorkspace.empty()) return;
    
    auto& config = m_workspaces[m_currentWorkspace];
    config.windowSizes[windowName] = {width, height};
}

std::pair<float, float> WorkspaceManager::getWindowPosition(const std::string& windowName) const {
    if (m_currentWorkspace.empty()) return {0, 0};
    
    auto it = m_workspaces.find(m_currentWorkspace);
    if (it == m_workspaces.end()) return {0, 0};
    
    auto posIt = it->second.windowPositions.find(windowName);
    return posIt != it->second.windowPositions.end() ? posIt->second : std::make_pair(0.0f, 0.0f);
}

std::pair<float, float> WorkspaceManager::getWindowSize(const std::string& windowName) const {
    if (m_currentWorkspace.empty()) return {400, 300};
    
    auto it = m_workspaces.find(m_currentWorkspace);
    if (it == m_workspaces.end()) return {400, 300};
    
    auto sizeIt = it->second.windowSizes.find(windowName);
    return sizeIt != it->second.windowSizes.end() ? sizeIt->second : std::make_pair(400.0f, 300.0f);
}

void WorkspaceManager::saveCurrentImGuiLayout() {
    // Save current ImGui state to the main .ini file
    ImGui::SaveIniSettingsToDisk(m_mainIniPath.c_str());
}

void WorkspaceManager::loadImGuiLayout(const std::string& layoutData) {
    if (layoutData.empty()) return;
    
    // Write the layout data to a temporary file
    std::string tempIniPath = m_mainIniPath + ".temp";
    std::ofstream tempFile(tempIniPath);
    if (tempFile.is_open()) {
        tempFile << layoutData;
        tempFile.close();
        
        // Load the layout from the temporary file
        ImGui::LoadIniSettingsFromDisk(tempIniPath.c_str());
        
        // Clean up temporary file
        std::filesystem::remove(tempIniPath);
    }
}

std::string WorkspaceManager::getCurrentImGuiLayout() const {
    // Read the current ImGui .ini file content
    std::ifstream file(m_mainIniPath);
    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    return "";
}

void WorkspaceManager::createDefaultWorkspaces() {
    // Creative Coding workspace - Code editor on left, Canvas on right
    WorkspaceConfig creativeCoding;
    creativeCoding.name = "Creative Coding";
    creativeCoding.description = "Optimized for creative coding and sketching - Code editor on left, Canvas on right";
    creativeCoding.defaultWindows = {"CodeEditor", "Canvas", "Toolbar", "Info", "Properties"};
    creativeCoding.windowVisibility = {
        {"CodeEditor", true},
        {"Canvas", true},
        {"Toolbar", true},
        {"Info", true},
        {"Properties", true},
        {"Stroke", false},
        {"AddonManager", false},
        {"NodeEditor", false},
        {"ThemeEditor", false},
        {"Texture", false}
    };
    m_workspaces["creative_coding"] = creativeCoding;
    
    // Illustrator workspace - Like Adobe Illustrator with toolbar and stroke panels
    WorkspaceConfig illustrator;
    illustrator.name = "Illustrator";
    illustrator.description = "Optimized for illustration and design work - Like Adobe Illustrator";
    illustrator.defaultWindows = {"Canvas", "Toolbar", "Stroke", "Properties", "Info"};
    illustrator.windowVisibility = {
        {"CodeEditor", false},
        {"Canvas", true},
        {"Toolbar", true},
        {"Info", true},
        {"Properties", true},
        {"Stroke", true},
        {"AddonManager", false},
        {"NodeEditor", false},
        {"ThemeEditor", false},
        {"Texture", false}
    };
    m_workspaces["illustrator"] = illustrator;
    
    // Save default workspace configs
    for (const auto& [name, config] : m_workspaces) {
        saveWorkspaceConfig(name);
    }
}

void WorkspaceManager::ensureWorkspaceDirectory() {
    if (!std::filesystem::exists(m_workspaceDir)) {
        std::filesystem::create_directories(m_workspaceDir);
    }
}

std::string WorkspaceManager::getWorkspaceConfigPath(const std::string& workspaceName) const {
    return m_workspaceDir + "/" + workspaceName + ".json";
}

bool WorkspaceManager::loadWorkspaceConfig(const std::string& workspaceName) {
    std::string configPath = getWorkspaceConfigPath(workspaceName);
    if (!std::filesystem::exists(configPath)) {
        return false;
    }
    
    try {
        std::ifstream file(configPath);
        nlohmann::json j;
        file >> j;
        
        WorkspaceConfig& config = m_workspaces[workspaceName];
        config.name = j.value("name", workspaceName);
        config.description = j.value("description", "");
        
        // Load default windows
        config.defaultWindows.clear();
        if (j.contains("defaultWindows")) {
            for (const auto& window : j["defaultWindows"]) {
                config.defaultWindows.push_back(window.get<std::string>());
            }
        }
        
        // Load window visibility
        config.windowVisibility.clear();
        if (j.contains("windowVisibility")) {
            for (const auto& [window, visible] : j["windowVisibility"].items()) {
                config.windowVisibility[window] = visible.get<bool>();
            }
        }
        
        // Load ImGui layout
        config.imguiLayout = j.value("imguiLayout", "");
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading workspace config: " << e.what() << std::endl;
        return false;
    }
}

bool WorkspaceManager::saveWorkspaceConfig(const std::string& workspaceName) {
    auto it = m_workspaces.find(workspaceName);
    if (it == m_workspaces.end()) {
        return false;
    }
    
    const WorkspaceConfig& config = it->second;
    
    try {
        nlohmann::json j;
        j["name"] = config.name;
        j["description"] = config.description;
        
        // Save default windows
        j["defaultWindows"] = config.defaultWindows;
        
        // Save window visibility
        j["windowVisibility"] = config.windowVisibility;
        
        // Save window positions
        nlohmann::json positions;
        for (const auto& [window, pos] : config.windowPositions) {
            positions[window] = {pos.first, pos.second};
        }
        j["windowPositions"] = positions;
        
        // Save window sizes
        nlohmann::json sizes;
        for (const auto& [window, size] : config.windowSizes) {
            sizes[window] = {size.first, size.second};
        }
        j["windowSizes"] = sizes;
        
        // Save ImGui layout
        j["imguiLayout"] = config.imguiLayout;
        
        std::string configPath = getWorkspaceConfigPath(workspaceName);
        std::ofstream file(configPath);
        file << j.dump(2);
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving workspace config: " << e.what() << std::endl;
        return false;
    }
}

void WorkspaceManager::loadExistingWorkspaces() {
    std::cout << "Loading existing workspaces from disk..." << std::endl;
    try {
        if (!std::filesystem::exists(m_workspaceDir)) {
            std::cout << "Workspace directory does not exist!" << std::endl;
            return;
        }
        // Scan the workspace directory for .json files
        for (const auto& entry : std::filesystem::directory_iterator(m_workspaceDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string workspaceName = entry.path().stem().string();
                std::cout << "Found workspace file: " << workspaceName << std::endl;
                // Try to load the workspace configuration
                if (loadWorkspaceConfig(workspaceName)) {
                    std::cout << "Successfully loaded workspace: " << workspaceName << std::endl;
                } else {
                    std::cout << "Failed to load workspace: " << workspaceName << std::endl;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in loadExistingWorkspaces: " << e.what() << std::endl;
    }
    std::cout << "Finished loading existing workspaces" << std::endl;
}

void WorkspaceManager::updateMainIniFile() {
    // This method can be called to ensure the main .ini file is up to date
    // It's called automatically when loading workspaces
}

} // namespace blot 