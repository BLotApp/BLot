#pragma once

#include "Window.h"
#include "../../ecs/systems/EventSystem.h"
#include "../../canvas/Canvas.h"
#include "../../canvas/CanvasManager.h"
#include <functional>
#include <vector>
#include <string>
#include <memory>

// Forward declarations
class ECSManager;

namespace blot {

// Forward declarations
class CodeEditorWindow;
class UIManager;

class MainMenuBar {
public:
    MainMenuBar(const std::string& title);
    ~MainMenuBar() = default;

    // Set the event system
    void setEventSystem(systems::EventSystem* eventSystem);
    
    // Set CodeEditorWindow reference for theme state
    void setCodeEditorWindow(std::shared_ptr<blot::CodeEditorWindow> window) { m_codeEditorWindow = window; }
    
    // Set Canvas reference for renderer state
    void setCanvas(std::shared_ptr<::Canvas> canvas) { m_canvas = canvas; }
    
    // Set CanvasManager reference for canvas operations
    void setCanvasManager(blot::CanvasManager* canvasManager) { m_canvasManager = canvasManager; }
    
    // Set UIManager reference for ImGui theme
    void setUIManager(UIManager* uiManager) { m_uiManager = uiManager; }
    
    // State setters (for menu state)
    void setActiveCanvasId(uint32_t id) { m_activeCanvasId = id; }
    void setCanvasEntities(const std::vector<std::pair<uint32_t, std::string>>& entities) { m_canvasEntities = entities; }

    void render();

private:
    std::string m_title;
    // Event system reference
    systems::EventSystem* m_eventSystem = nullptr;
    
    // CodeEditorWindow reference for theme state
    std::shared_ptr<blot::CodeEditorWindow> m_codeEditorWindow;
    
    // Canvas reference for renderer state
    std::shared_ptr<::Canvas> m_canvas;
    
    // CanvasManager reference for canvas operations
    blot::CanvasManager* m_canvasManager = nullptr;
    
    // UIManager reference for ImGui theme
    UIManager* m_uiManager = nullptr;
    
    // State variables
    uint32_t m_activeCanvasId = 0;
    std::vector<std::pair<uint32_t, std::string>> m_canvasEntities;
    
    // Helper methods
    void triggerAction(const std::string& actionId);
    bool hasAction(const std::string& actionId) const;
};

} // namespace blot 