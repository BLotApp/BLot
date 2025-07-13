#pragma once

#include "Window.h"
#include "../../ecs/systems/EventSystem.h"
#include <functional>
#include <vector>
#include <string>
#include <memory>

// Forward declarations
class ECSManager;

namespace blot {

class MainMenuBar : public Window {
public:
    MainMenuBar(const std::string& title, Window::Flags flags = Window::Flags::None);
    ~MainMenuBar() = default;

    // Set the event system
    void setEventSystem(systems::EventSystem* eventSystem) { m_eventSystem = eventSystem; }
    
    // State setters (for menu state)
    void setCurrentTheme(int theme) { m_currentTheme = theme; }
    void setCurrentRendererType(int type) { m_currentRendererType = type; }
    void setActiveCanvasId(uint32_t id) { m_activeCanvasId = id; }
    void setCanvasEntities(const std::vector<std::pair<uint32_t, std::string>>& entities) { m_canvasEntities = entities; }

protected:
    void render() override;

private:
    // Event system reference
    systems::EventSystem* m_eventSystem = nullptr;
    
    // State variables
    int m_currentRendererType = 0;
    int m_currentTheme = 0;
    uint32_t m_activeCanvasId = 0;
    std::vector<std::pair<uint32_t, std::string>> m_canvasEntities;
    
    // Helper methods
    void triggerAction(const std::string& actionId);
    bool hasAction(const std::string& actionId) const;
};

} // namespace blot 