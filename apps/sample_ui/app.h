#pragma once

#include "core/IApp.h"
#include "core/BlotEngine.h"
#include "ecs/ECSManager.h"
#include "core/canvas/CanvasManager.h"
#include "rendering/RenderingManager.h"
#include "ui/UIManager.h"
#include "core/util/SettingsManager.h"
#include <GLFW/glfw3.h>
#include <entt/entity/entity.hpp>

// Forward declarations
class bxCodeEditor;
class bxScriptEngine;
namespace blot {
    class AddonManager;
    namespace systems { class EventSystem; }
    class MainMenuBar;
}

namespace blot { class BlotEngine; class IApp; }

class SampleUiApp : public blot::IApp {
public:
    SampleUiApp();
    ~SampleUiApp();

    void setup(blot::BlotEngine* engine) override;
    void update(float deltaTime) override;
    void draw() override;
    void configureWindow(WindowSettings& settings) override;

    // Accessors to core managers and utilities
    blot::ECSManager* getECSManager();
    blot::RenderingManager* getRenderingManager();
    blot::CanvasManager* getCanvasManager();
    blot::UIManager* getUIManager();
    blot::AddonManager* getAddonManager();
    bxScriptEngine* getScriptEngine();
    blot::SettingsManager& getSettings();
    const blot::SettingsManager& getSettings() const;
    bxCodeEditor* getCodeEditor();

private:
    // Internal helper methods
    void connectEventSystemToUI();
    void connectAddonManagerToEventSystem(blot::systems::EventSystem& eventSystem);
    void registerUIActions(blot::systems::EventSystem& eventSystem);

    // Cached engine pointer populated in setup()
    blot::BlotEngine* m_engine = nullptr;

    // Core window reference (from GLFW)
    GLFWwindow* m_window;

    // Application state
    int m_windowWidth;
    int m_windowHeight;
    bool m_running;
    float m_deltaTime;
    float m_lastFrameTime;

    // Active canvas tracking
    entt::entity m_activeCanvasId{ entt::null };

    // Addons
    bxCodeEditor* m_codeEditor = nullptr;
}; 