#pragma once

#include "core/core.h"

// Forward declare addons
class bxScriptEngine;
class bxCodeEditor;

class ExampleApp : public blot::IApp {
  public:
	ExampleApp() {
		window().width = 1024;
		window().height = 768;
		window().title = "Example App";
	}

	void setup() override;
	void update(float deltaTime) override;
	void draw() override;

  private:
	// Addon accessor helpers
	bxScriptEngine *getScriptEngine();
	bxCodeEditor *getCodeEditor();

	// Internal helper methods
	void connectEventSystemToUI();
	void connectAddonManagerToEventSystem(blot::ecs::SEvent &eventSystem);
	void registerUIActions(blot::ecs::SEvent &eventSystem);

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
