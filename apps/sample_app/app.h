#pragma once

#include "rendering/gladGlfw.h"
#include <entt/entt.hpp>
#include "core/BlotEngine.h"
#include "core/IApp.h"
#include "core/canvas/CanvasManager.h"
#include "core/util/SettingsManager.h"
#include "ecs/ECSManager.h"
#include "rendering/RenderingManager.h"
#include "ui/UIManager.h"

// Forward declarations
class bxCodeEditor;
class bxScriptEngine;
namespace blot {
class AddonManager;
namespace systems {
class EventSystem;
}
class MainMenuBar;
} // namespace blot

namespace blot {
class BlotEngine;
class IApp;
} // namespace blot

class SampleUiApp : public blot::IApp {
  public:
	SampleUiApp();
	~SampleUiApp();

	void setup() override;
	void update(float deltaTime) override;
	void draw() override;

  private:
	// Addon accessor helpers
	bxScriptEngine *getScriptEngine();
	bxCodeEditor *getCodeEditor();

	// Internal helper methods
	void connectEventSystemToUI();
	void
	connectAddonManagerToEventSystem(blot::systems::EventSystem &eventSystem);
	void registerUIActions(blot::systems::EventSystem &eventSystem);

	// Core window reference (from GLFW)
	GLFWwindow *m_window;

	// Application state
	int m_windowWidth;
	int m_windowHeight;
	bool m_running;
	float m_deltaTime;
	float m_lastFrameTime;

	// Active canvas tracking
	entt::entity m_activeCanvasId{entt::null};

	// Addons
	bxCodeEditor *m_codeEditor = nullptr;
};
