#include "WindowManager.h"
#include "imgui.h"
#include <algorithm>
#include <iostream> // Added for debug output

namespace blot {

WindowManager::WindowManager() : m_focusedWindowEntity(entt::null) {
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
        const auto& settingsComp = view.get<WindowSettingsComponent>(entity);
        
        // Use the window name as both the ID and display name
        // In the future, we could add a display name field to WindowSettingsComponent
        std::string displayName = windowComp.name;
        
        // Make the display name more user-friendly
        if (displayName == "MainMenuBar") displayName = "Main Menu Bar";
        else if (displayName == "CodeEditor") displayName = "Code Editor";
        else if (displayName == "AddonManager") displayName = "Addon Manager";
        else if (displayName == "NodeEditor") displayName = "Node Editor";
        else if (displayName == "ThemeEditor") displayName = "Theme Editor";
        else if (displayName == "Texture") displayName = "Texture Viewer";
        
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
    // Sort windows by z-order
    sortWindowsByZOrder();
    
    // Render all visible windows
    auto view = m_registry.view<WindowComponent, WindowTransformComponent, WindowStyleComponent>();
    for (auto entity : view) {
        auto& windowComp = view.get<WindowComponent>(entity);
        auto& transformComp = view.get<WindowTransformComponent>(entity);
        auto& styleComp = view.get<WindowStyleComponent>(entity);
        
        if (windowComp.isVisible && windowComp.window) {
            std::cout << "[Frame] Rendering window: " << windowComp.name << std::endl;
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
    std::cout << "setWindowVisible: " << name << " -> " << (visible ? "visible" : "hidden") << std::endl;
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

void WindowManager::hideAllWindows() {
    auto view = m_registry.view<WindowComponent>();
    for (auto entity : view) {
        auto& windowComp = view.get<WindowComponent>(entity);
        windowComp.isVisible = false;
        if (windowComp.window) {
            windowComp.window->hide();
        }
    }
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

} // namespace blot 