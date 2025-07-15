#include "WindowManager.h"
#include "imgui.h"
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>
#include "AppPaths.h"

namespace blot {

WindowManager::WindowManager() : m_focusedWindowEntity(entt::null) {
    m_workspaceDir = AppPaths::getWorkspacesDir();
    m_mainIniPath = AppPaths::getImGuiIniPath();
    std::cout << "[DEBUG] WindowManager constructed, workspaceDir=" << m_workspaceDir << std::endl;
    loadExistingWorkspaces();
}

WindowManager::~WindowManager() {
    closeAllWindows();
}

entt::entity WindowManager::createWindow(const std::string& name, std::shared_ptr<Window> window) {
    // Check if window with this name already exists
    auto existingEntity = getWindowEntity(name);
    if (existingEntity != entt::null) {
        return existingEntity;
    }
    
    // Create new window entity
    auto entity = m_registry.create();
    
    // Add components
    m_registry.emplace<WindowComponent>(entity, window, name, true, false, 0);
    m_registry.emplace<WindowTransformComponent>(entity);
    m_registry.emplace<WindowStyleComponent>(entity);
    m_registry.emplace<WindowInputComponent>(entity);
    m_registry.emplace<WindowSettingsComponent>(entity);
    
    // If this is the first window, make it focused
    if (m_focusedWindowEntity == entt::null) {
        m_focusedWindowEntity = entity;
        auto& windowComp = m_registry.get<WindowComponent>(entity);
        windowComp.isFocused = true;
    }
    
    return entity;
}

void WindowManager::destroyWindow(entt::entity windowEntity) {
    if (m_registry.valid(windowEntity)) {
        // If we're destroying the focused window, clear focus
        if (windowEntity == m_focusedWindowEntity) {
            m_focusedWindowEntity = entt::null;
        }
        m_registry.destroy(windowEntity);
    }
}

void WindowManager::destroyWindow(const std::string& windowName) {
    auto entity = getWindowEntity(windowName);
    if (entity != entt::null) {
        destroyWindow(entity);
    }
}

entt::entity WindowManager::getWindowEntity(const std::string& name) {
    auto view = m_registry.view<WindowComponent>();
    for (auto entity : view) {
        auto& windowComp = view.get<WindowComponent>(entity);
        if (windowComp.name == name) {
            return entity;
        }
    }
    return entt::null;
}

std::shared_ptr<Window> WindowManager::getWindow(const std::string& name) {
    auto entity = getWindowEntity(name);
    if (entity != entt::null) {
        auto& windowComp = m_registry.get<WindowComponent>(entity);
        return windowComp.window;
    }
    return nullptr;
}

std::shared_ptr<Window> WindowManager::getFocusedWindow() {
    if (m_focusedWindowEntity != entt::null && m_registry.valid(m_focusedWindowEntity)) {
        auto& windowComp = m_registry.get<WindowComponent>(m_focusedWindowEntity);
        return windowComp.window;
    }
    return nullptr;
}

entt::entity WindowManager::getFocusedWindowEntity() {
    return m_focusedWindowEntity;
}

std::vector<std::string> WindowManager::getAllWindowNames() const {
    std::vector<std::string> windowNames;
    auto view = m_registry.view<WindowComponent>();
    for (auto entity : view) {
        const auto& windowComp = view.get<WindowComponent>(entity);
        windowNames.push_back(windowComp.name);
    }
    return windowNames;
}

std::vector<std::pair<std::string, std::string>> WindowManager::getAllWindowsWithDisplayNames() const {
    std::vector<std::pair<std::string, std::string>> windows;
    auto view = m_registry.view<WindowComponent, WindowSettingsComponent>();
    for (auto entity : view) {
        const auto& windowComp = view.get<WindowComponent>(entity);
        // Use the window's title for display, fallback to name if window is null
        std::string displayName = windowComp.window ? windowComp.window->getTitle() : windowComp.name;
        windows.push_back({windowComp.name, displayName});
    }
    return windows;
}

void WindowManager::showWindow(const std::string& name) {
    auto entity = getWindowEntity(name);
    if (entity != entt::null) {
        auto& windowComp = m_registry.get<WindowComponent>(entity);
        windowComp.isVisible = true;
        if (windowComp.window) {
            windowComp.window->show();
        }
    }
}

void WindowManager::hideWindow(const std::string& name) {
    auto entity = getWindowEntity(name);
    if (entity != entt::null) {
        auto& windowComp = m_registry.get<WindowComponent>(entity);
        windowComp.isVisible = false;
        if (windowComp.window) {
            windowComp.window->hide();
        }
    }
}

void WindowManager::closeWindow(const std::string& name) {
    auto entity = getWindowEntity(name);
    if (entity != entt::null) {
        auto& windowComp = m_registry.get<WindowComponent>(entity);
        if (windowComp.window) {
            windowComp.window->close();
        }
        destroyWindow(entity);
    }
}

void WindowManager::focusWindow(const std::string& name) {
    auto entity = getWindowEntity(name);
    if (entity != entt::null) {
        // Clear previous focus
        if (m_focusedWindowEntity != entt::null && m_registry.valid(m_focusedWindowEntity)) {
            auto& prevWindowComp = m_registry.get<WindowComponent>(m_focusedWindowEntity);
            prevWindowComp.isFocused = false;
        }
        
        // Set new focus
        m_focusedWindowEntity = entity;
        auto& windowComp = m_registry.get<WindowComponent>(entity);
        windowComp.isFocused = true;
    }
}

void WindowManager::closeFocusedWindow() {
    if (m_focusedWindowEntity != entt::null && m_registry.valid(m_focusedWindowEntity)) {
        auto& windowComp = m_registry.get<WindowComponent>(m_focusedWindowEntity);
        if (windowComp.window) {
            windowComp.window->close();
        }
        destroyWindow(m_focusedWindowEntity);
        updateFocus();
    }
}

void WindowManager::closeAllWindows() {
    auto view = m_registry.view<WindowComponent>();
    for (auto entity : view) {
        auto& windowComp = view.get<WindowComponent>(entity);
        if (windowComp.window) {
            windowComp.window->close();
        }
    }
    m_registry.clear();
    m_focusedWindowEntity = entt::null;
}

void WindowManager::renderAllWindows() {
    std::cout << "[WindowManager] renderAllWindows() called" << std::endl;
    // Sort windows by z-order
    sortWindowsByZOrder();
    
    // Render all visible windows
    auto view = m_registry.view<WindowComponent, WindowTransformComponent, WindowStyleComponent>();
    for (auto entity : view) {
        auto& windowComp = view.get<WindowComponent>(entity);
        auto& transformComp = view.get<WindowTransformComponent>(entity);
        auto& styleComp = view.get<WindowStyleComponent>(entity);
        
        if (windowComp.isVisible && windowComp.window) {
            // Apply transform and style
            windowComp.window->setPosition(transformComp.position);
            windowComp.window->setSize(transformComp.size);
            windowComp.window->setMinSize(transformComp.minSize);
            windowComp.window->setMaxSize(transformComp.maxSize);
            windowComp.window->setAlpha(styleComp.alpha);
            windowComp.window->setFlags(static_cast<Window::Flags>(styleComp.flags));
            
            // Render the window
            windowComp.window->render();
        }
    }
}

void WindowManager::handleInput() {
    // Handle ESC key to close focused window
    handleEscapeKey();
    
    // Update focus based on ImGui's focused window
    updateFocus();
}

void WindowManager::update() {
    // Update all window states
    auto view = m_registry.view<WindowComponent>();
    for (auto entity : view) {
        auto& windowComp = view.get<WindowComponent>(entity);
        if (windowComp.window) {
            // Update window state from ImGui
            windowComp.isFocused = windowComp.window->isFocused();
            windowComp.isVisible = windowComp.window->isVisible();
        }
    }
    
    // Handle input
    handleInput();
}

void WindowManager::updateFocus() {
    // Find the currently focused window in ImGui
    entt::entity newFocusedEntity = entt::null;
    
    auto view = m_registry.view<WindowComponent>();
    for (auto entity : view) {
        auto& windowComp = view.get<WindowComponent>(entity);
        if (windowComp.window && windowComp.isFocused) {
            newFocusedEntity = entity;
            break;
        }
    }
    
    // Update focus state
    if (newFocusedEntity != m_focusedWindowEntity) {
        // Clear previous focus
        if (m_focusedWindowEntity != entt::null && m_registry.valid(m_focusedWindowEntity)) {
            auto& prevWindowComp = m_registry.get<WindowComponent>(m_focusedWindowEntity);
            prevWindowComp.isFocused = false;
        }
        
        // Set new focus
        m_focusedWindowEntity = newFocusedEntity;
        if (newFocusedEntity != entt::null) {
            auto& windowComp = m_registry.get<WindowComponent>(newFocusedEntity);
            windowComp.isFocused = true;
        }
    }
}

void WindowManager::handleEscapeKey() {
    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        // Find the focused window that should close on ESC
        auto view = m_registry.view<WindowComponent, WindowInputComponent>();
        for (auto entity : view) {
            auto& windowComp = view.get<WindowComponent>(entity);
            auto& inputComp = view.get<WindowInputComponent>(entity);
            
            if (windowComp.isFocused && inputComp.closeOnEscape) {
                closeFocusedWindow();
                break;
            }
        }
    }
}

void WindowManager::sortWindowsByZOrder() {
    // This would sort windows by z-order for proper rendering
    // For now, we'll use the default entity order
    // In a more complex implementation, you might want to sort the view
}

// Window visibility management
bool WindowManager::isWindowVisible(const std::string& name) {
    auto entity = getWindowEntity(name);
    if (entity != entt::null) {
        auto& windowComp = m_registry.get<WindowComponent>(entity);
        return windowComp.isVisible;
    }
    return false;
}

void WindowManager::setWindowVisible(const std::string& name, bool visible) {
    auto entity = getWindowEntity(name);
    if (entity != entt::null) {
        auto& windowComp = m_registry.get<WindowComponent>(entity);
        windowComp.isVisible = visible;
        if (windowComp.window) {
            if (visible) {
                windowComp.window->show();
            } else {
                windowComp.window->hide();
            }
        }
    }
}

void WindowManager::toggleWindow(const std::string& name) {
    auto entity = getWindowEntity(name);
    if (entity != entt::null) {
        auto& windowComp = m_registry.get<WindowComponent>(entity);
        windowComp.isVisible = !windowComp.isVisible;
        if (windowComp.window) {
            if (windowComp.isVisible) {
                windowComp.window->show();
            } else {
                windowComp.window->hide();
            }
        }
    }
}

void WindowManager::showAllWindows() {
    auto view = m_registry.view<WindowComponent>();
    for (auto entity : view) {
        auto& windowComp = view.get<WindowComponent>(entity);
        windowComp.isVisible = true;
        if (windowComp.window) {
            windowComp.window->show();
        }
    }
}

void WindowManager::hideAllWindows(const std::vector<std::string>& except) {
    auto allWindows = getAllWindowNames();
    for (const auto& name : allWindows) {
        if (std::find(except.begin(), except.end(), name) != except.end()) continue;
        setWindowVisible(name, false);
    }
}

void WindowManager::setMainMenuBar(bool visible) {
    setWindowVisible("MainMenuBar", visible);
}

// Window settings management
void WindowManager::setWindowSettings(const std::string& name, const WindowSettingsComponent& settings) {
    auto entity = getWindowEntity(name);
    if (entity != entt::null) {
        if (m_registry.all_of<WindowSettingsComponent>(entity)) {
            m_registry.replace<WindowSettingsComponent>(entity, settings);
        } else {
            m_registry.emplace<WindowSettingsComponent>(entity, settings);
        }
    }
}

WindowSettingsComponent WindowManager::getWindowSettings(const std::string& name) {
    auto entity = getWindowEntity(name);
    if (entity != entt::null && m_registry.all_of<WindowSettingsComponent>(entity)) {
        return m_registry.get<WindowSettingsComponent>(entity);
    }
    return WindowSettingsComponent{}; // Return default settings
}

std::vector<std::string> WindowManager::getVisibleWindows() {
    std::vector<std::string> visibleWindows;
    auto view = m_registry.view<WindowComponent>();
    for (auto entity : view) {
        auto& windowComp = view.get<WindowComponent>(entity);
        if (windowComp.isVisible) {
            visibleWindows.push_back(windowComp.name);
        }
    }
    return visibleWindows;
}

std::vector<std::string> WindowManager::getHiddenWindows() {
    std::vector<std::string> hiddenWindows;
    auto view = m_registry.view<WindowComponent>();
    for (auto entity : view) {
        auto& windowComp = view.get<WindowComponent>(entity);
        if (!windowComp.isVisible) {
            hiddenWindows.push_back(windowComp.name);
        }
    }
    return hiddenWindows;
}

std::vector<std::string> WindowManager::getWindowsByCategory(const std::string& category) {
    std::vector<std::string> categoryWindows;
    auto view = m_registry.view<WindowComponent, WindowSettingsComponent>();
    for (auto entity : view) {
        auto& windowComp = view.get<WindowComponent>(entity);
        auto& settingsComp = view.get<WindowSettingsComponent>(entity);
        if (settingsComp.category == category) {
            categoryWindows.push_back(windowComp.name);
        }
    }
    return categoryWindows;
}

// Menu integration
void WindowManager::renderWindowMenu() {
    if (ImGui::BeginMenu("Windows")) {
        auto view = m_registry.view<WindowComponent, WindowSettingsComponent>();
        for (auto entity : view) {
            auto& windowComp = view.get<WindowComponent>(entity);
            auto& settingsComp = view.get<WindowSettingsComponent>(entity);
            
            if (settingsComp.showInMenu) {
                bool isVisible = windowComp.isVisible;
                if (ImGui::MenuItem(windowComp.name.c_str(), nullptr, &isVisible)) {
                    setWindowVisible(windowComp.name, isVisible);
                }
            }
        }
        ImGui::EndMenu();
    }
}

std::vector<std::string> WindowManager::getMenuWindows() {
    std::vector<std::string> menuWindows;
    auto view = m_registry.view<WindowComponent, WindowSettingsComponent>();
    for (auto entity : view) {
        auto& windowComp = view.get<WindowComponent>(entity);
        auto& settingsComp = view.get<WindowSettingsComponent>(entity);
        if (settingsComp.showInMenu) {
            menuWindows.push_back(windowComp.name);
        }
    }
    return menuWindows;
}

// Workspace management methods
bool WindowManager::loadWorkspace(const std::string& workspaceName) {
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
    std::set<std::string> missingWindows;
    const auto& config = m_workspaces[workspaceName];
    // Apply window visibility from workspace
    std::vector<std::string> allWindows = getAllWindowNames();
    std::set<std::string> allWindowSet(allWindows.begin(), allWindows.end());
    for (const auto& windowName : allWindows) {
        std::cout << "  Setting " << windowName << " to visible (default)" << std::endl;
        setWindowVisible(windowName, true);
    }
    for (const auto& [windowName, isVisible] : config.windowVisibility) {
        if (allWindowSet.count(windowName)) {
            if (!isVisible) {
                std::cout << "  Setting " << windowName << " to hidden" << std::endl;
                setWindowVisible(windowName, false);
            }
        } else {
            std::cerr << "[Workspace] Could not find window named '" << windowName << "' referenced in workspace. Skipping." << std::endl;
            missingWindows.insert(windowName);
        }
    }
    // TODO: Show modal for missing windows if needed (UIManager responsibility)
    if (!config.imguiLayout.empty()) {
        std::cout << "Loading ImGui layout from workspace..." << std::endl;
        // loadImGuiLayout(config.imguiLayout); // Enable if needed
        std::cout << "ImGui layout loading temporarily disabled for debugging" << std::endl;
    } else {
        std::cout << "No ImGui layout found in workspace, using default" << std::endl;
    }
    m_currentWorkspace = workspaceName;
    std::cout << "Loaded workspace: " << workspaceName << std::endl;
    return true;
}

bool WindowManager::saveWorkspace(const std::string& workspaceName) {
    WorkspaceConfig currentState = captureCurrentUIState(workspaceName);
    m_workspaces[workspaceName] = currentState;
    if (!saveWorkspaceConfig(workspaceName)) {
        std::cerr << "Failed to save workspace config for '" << workspaceName << "'" << std::endl;
        return false;
    }
    std::cout << "Saved workspace: " << workspaceName << std::endl;
    return true;
}

bool WindowManager::saveWorkspaceAs(const std::string& workspaceName) {
    WorkspaceConfig newConfig = captureCurrentUIState(workspaceName);
    m_workspaces[workspaceName] = newConfig;
    return saveWorkspace(workspaceName);
}

WorkspaceConfig WindowManager::captureCurrentUIState(const std::string& workspaceName) {
    std::cout << "Capturing current UI state for workspace: " << workspaceName << std::endl;
    WorkspaceConfig config;
    config.name = workspaceName;
    config.description = "Custom workspace created on " + std::to_string(std::time(nullptr));
    std::vector<std::string> allWindows = getAllWindowNames();
    std::cout << "Found " << allWindows.size() << " windows:" << std::endl;
    for (const auto& windowName : allWindows) {
        bool isVisible = isWindowVisible(windowName);
        if (!isVisible) {
            config.windowVisibility[windowName] = false;
            std::cout << "  - " << windowName << ": hidden (captured)" << std::endl;
        } else {
            std::cout << "  - " << windowName << ": visible (not captured)" << std::endl;
        }
    }
    config.imguiLayout = getCurrentImGuiLayout();
    std::cout << "ImGui layout captured: " << (config.imguiLayout.empty() ? "empty" : "non-empty") << std::endl;
    std::cout << "UI state capture complete" << std::endl;
    return config;
}

WorkspaceConfig WindowManager::createWorkspaceFromCurrentState(const std::string& workspaceName) {
    return captureCurrentUIState(workspaceName);
}

bool WindowManager::createWorkspace(const std::string& workspaceName, const WorkspaceConfig& config) {
    if (m_workspaces.find(workspaceName) != m_workspaces.end()) {
        std::cerr << "Workspace '" << workspaceName << "' already exists" << std::endl;
        return false;
    }
    m_workspaces[workspaceName] = config;
    return saveWorkspaceConfig(workspaceName);
}

bool WindowManager::deleteWorkspace(const std::string& workspaceName) {
    if (m_workspaces.find(workspaceName) == m_workspaces.end()) {
        return false;
    }
    std::filesystem::remove(getWorkspaceConfigPath(workspaceName));
    m_workspaces.erase(workspaceName);
    return true;
}

std::vector<std::string> WindowManager::getAvailableWorkspaces() const {
    std::vector<std::string> workspaces;
    for (const auto& [name, config] : m_workspaces) {
        workspaces.push_back(name);
    }
    return workspaces;
}

std::vector<std::pair<std::string, std::string>> WindowManager::getAvailableWorkspacesWithNames() const {
    std::vector<std::pair<std::string, std::string>> workspaces;
    for (const auto& [name, config] : m_workspaces) {
        workspaces.push_back({name, config.name});
    }
    return workspaces;
}

WorkspaceConfig WindowManager::getWorkspaceConfig(const std::string& workspaceName) const {
    auto it = m_workspaces.find(workspaceName);
    if (it != m_workspaces.end()) {
        return it->second;
    }
    return WorkspaceConfig{};
}

void WindowManager::setWindowVisibility(const std::string& windowName, bool visible) {
    if (m_currentWorkspace.empty()) return;
    auto& config = m_workspaces[m_currentWorkspace];
    config.windowVisibility[windowName] = visible;
}

bool WindowManager::getWindowVisibility(const std::string& windowName) const {
    if (m_currentWorkspace.empty()) return true;
    auto it = m_workspaces.find(m_currentWorkspace);
    if (it == m_workspaces.end()) return true;
    auto visibilityIt = it->second.windowVisibility.find(windowName);
    return visibilityIt != it->second.windowVisibility.end() ? visibilityIt->second : true;
}

void WindowManager::saveCurrentImGuiLayout() {
    ImGui::SaveIniSettingsToDisk(m_mainIniPath.c_str());
}

void WindowManager::loadImGuiLayout(const std::string& layoutData) {
    if (layoutData.empty()) return;
    std::string tempIniPath = m_mainIniPath + ".temp";
    std::ofstream tempFile(tempIniPath);
    if (tempFile.is_open()) {
        tempFile << layoutData;
        tempFile.close();
        ImGui::LoadIniSettingsFromDisk(tempIniPath.c_str());
        std::filesystem::remove(tempIniPath);
    }
}

std::string WindowManager::getCurrentImGuiLayout() const {
    std::ifstream file(m_mainIniPath);
    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    return "";
}

void WindowManager::ensureWorkspaceDirectory() {
    if (!std::filesystem::exists(m_workspaceDir)) {
        std::filesystem::create_directories(m_workspaceDir);
    }
}

void WindowManager::createDefaultWorkspaces() {
    std::cout << "Workspaces are loaded dynamically from JSON files" << std::endl;
}

void WindowManager::loadExistingWorkspaces() {
    std::cout << "[DEBUG] Looking for workspaces in: " << std::filesystem::absolute(m_workspaceDir) << std::endl;
    try {
        if (!std::filesystem::exists(m_workspaceDir)) {
            std::cout << "[DEBUG] Workspace directory does not exist: " << m_workspaceDir << std::endl;
            return;
        }
        for (const auto& entry : std::filesystem::directory_iterator(m_workspaceDir)) {
            std::cout << "[DEBUG] Found file: " << entry.path() << std::endl;
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string workspaceName = entry.path().stem().string();
                std::cout << "Found workspace file: " << workspaceName << std::endl;
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

std::string WindowManager::getWorkspaceConfigPath(const std::string& workspaceName) const {
    return m_workspaceDir + "/" + workspaceName + ".json";
}

bool WindowManager::loadWorkspaceConfig(const std::string& workspaceName) {
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
        config.windowVisibility.clear();
        if (j.contains("windowVisibility")) {
            for (const auto& [window, visible] : j["windowVisibility"].items()) {
                config.windowVisibility[window] = visible.get<bool>();
            }
        }
        config.imguiLayout = j.value("imguiLayout", "");
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading workspace config: " << e.what() << std::endl;
        return false;
    }
}

bool WindowManager::saveWorkspaceConfig(const std::string& workspaceName) {
    auto it = m_workspaces.find(workspaceName);
    if (it == m_workspaces.end()) return false;
    const WorkspaceConfig& config = it->second;
    nlohmann::json j;
    j["name"] = config.name;
    j["description"] = config.description;
    j["windowVisibility"] = config.windowVisibility;
    j["imguiLayout"] = config.imguiLayout;
    std::string configPath = getWorkspaceConfigPath(workspaceName);
    std::ofstream file(configPath);
    if (file.is_open()) {
        file << j.dump(2);
        return true;
    }
    return false;
}

void WindowManager::updateMainIniFile() {
    // This method can be called to ensure the main .ini file is up to date
    // It's called automatically when loading workspaces
}

} // namespace blot 