#pragma once
#include <string>

// WindowSettings struct
#include "core/AppSettings.h"
#include "core/WindowSettings.h"
#include "ecs/systems/SEvent.h"
namespace blot {

class BlotEngine;
class MEcs;
class MAddon;
class MRendering;
class MCanvas;
class MSettings;
class Iui;

class IApp {
  public:
	virtual ~IApp() = default;

	// ----------------------------------------
	// Framework entry points DO NOT override.
	// ----------------------------------------
	void blotSetup(blot::BlotEngine *engine);
	void blotUpdate(float deltaTime);
	void blotDraw();

	// Engine pointer helpers
	void setEngine(blot::BlotEngine *engine) { m_engine = engine; }
	blot::BlotEngine *getEngine() const { return m_engine; }

	// Access full application settings before engine creation
	AppSettings &settings() { return m_settings; }
	const AppSettings &settings() const { return m_settings; }

	// Convenience aliases for window settings (legacy API)
	WindowSettings &window() { return m_settings.window; }
	const WindowSettings &window() const { return m_settings.window; }

	// Application state helpers
	int getWindowWidth() const { return m_settings.window.width; }
	int getWindowHeight() const { return m_settings.window.height; }
	float getDeltaTime() const { return m_deltaTime; }

  protected:
	// ----------------------------------------
	// User hooks – override these in your app
	// ----------------------------------------
	virtual void setup() {}
	virtual void update(float) {}
	virtual void draw() {}

	// Derived apps can register UI/Event actions here after engine setup
	virtual void registerUIActions(blot::ecs::SEvent &) {}

	// Convenience accessors
	blot::MEcs *getECSManager() const;
	blot::MRendering *getRenderingManager() const;
	blot::MCanvas *getCanvasManager() const;
	blot::Iui *getUiManager() const;
	blot::MAddon *getAddonManager() const;
	blot::MSettings *getSettingsManager() const;

	// Convenience: access engine frame counter
	uint64_t frameCount() const;

	blot::BlotEngine *m_engine = nullptr;
	AppSettings m_settings;
	float m_deltaTime = 0.0f;
	float m_lastFrameTime = 0.0f;
};
} // namespace blot
