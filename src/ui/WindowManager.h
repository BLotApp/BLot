#pragma once

#include <memory>
#include <vector>
#include <string>
#include <entt/entt.hpp>
#include "Window.h"

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
    
    // Window operations
    void showWindow(const std::string& name);
    void hideWindow(const std::string& name);
    void closeWindow(const std::string& name);
    void focusWindow(const std::string& name);
    void closeFocusedWindow();
    void closeAllWindows();
    
    // Rendering and input
    void renderAllWindows();
    void handleInput();
    void update();
    
    // Registry access
    entt::registry& getRegistry() { return m_registry; }
    const entt::registry& getRegistry() const { return m_registry; }

private:
    entt::registry m_registry;
    entt::entity m_focusedWindowEntity = entt::null;
    
    void updateFocus();
    void handleEscapeKey();
    void sortWindowsByZOrder();
};

} // namespace blot 