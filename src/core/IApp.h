#pragma once
#include <string>

// WindowSettings struct
#include "core/WindowSettings.h"
namespace blot {

class BlotEngine;
class ECSManager;
class AddonManager;
class UIManager;
class RenderingManager;
class CanvasManager;
class SettingsManager;

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

	// Access window settings (can be modified in constructor before engine
	// creation)
	WindowSettings &window() { return settings_; }
	const WindowSettings &window() const { return settings_; }

  protected:
	// ----------------------------------------
	// User hooks – override these in your app
	// ----------------------------------------
	virtual void setup() {}
	virtual void update(float) {}
	virtual void draw() {}

	// Convenience accessors
	blot::ECSManager *getECSManager() const;
	blot::RenderingManager *getRenderingManager() const;
	blot::CanvasManager *getCanvasManager() const;
	blot::UIManager *getUIManager() const;
	blot::AddonManager *getAddonManager() const;
	blot::SettingsManager *getSettingsManager() const;

	// Convenience: access engine frame counter
	uint64_t frameCount() const;

	blot::BlotEngine *m_engine = nullptr;
	WindowSettings settings_;
};
} // namespace blot
