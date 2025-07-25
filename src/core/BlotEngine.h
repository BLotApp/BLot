#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "core/AppSettings.h"
#include "core/Iui.h"
#include "core/U_core.h"
#include "core/WindowSettings.h"
#include "rendering/U_gladGlfw.h"

// Forward declarations for all managers and IApp
namespace blot {
class IApp;
class MEcs;
class MAddon;
class Iui;
// Mui is now managed through addons, not directly
class MRendering;
class MCanvas;
class MSettings;
} // namespace blot

namespace blot {
class BlotEngine {
  public:
	BlotEngine(std::unique_ptr<IApp> app);
	BlotEngine(std::unique_ptr<IApp> app, const AppSettings &settings);
	~BlotEngine();
	void init(const std::string &appName, float appVersion);
	void run();
	void setAppName(const std::string &name) { m_appName = name; }
	void setAppVersion(float version) { m_appVersion = version; }
	const std::string &getAppName() const { return m_appName; }
	float getAppVersion() const { return m_appVersion; }

	MEcs *getECSManager() { return m_ecsManager.get(); }
	MAddon *getAddonManager() { return m_addonManager.get(); }
	// UI manager access through addon system
	// Mui *getUIManager() { return reinterpret_cast<Mui *>(m_uiManager.get());
	// }
	Iui *getUiManager() { return m_uiManager.get(); }
	MRendering *getRenderingManager() { return m_renderingManager.get(); }
	MCanvas *getCanvasManager() { return m_canvasManager.get(); }
	MSettings *getSettings() { return m_settingsManager.get(); }

	void setDebugMode(bool enabled) { m_debugMode = enabled; }
	bool getDebugMode() const { return m_debugMode; }
	void toggleDebugMode() { m_debugMode = !m_debugMode; }

	// V-Sync and frame-rate control
	void setVerticalSync(bool enabled);
	bool getVerticalSync() const { return m_vsync; }
	void setTargetFrameRate(int fps);
	int getTargetFrameRate() const { return m_targetFps; }
	int getCurrentFrameRate() const { return m_currentFps; }

	// Frame counter (increments once per main loop iteration)
	uint64_t getFrameCount() const { return m_frameCount; }

	// Background clear colour used each frame (for apps without a Canvas)
	void setClearColor(float r, float g, float b, float a = 1.0f) {
		m_clearColor = glm::vec4(r, g, b, a);
	}
	glm::vec4 getClearColor() const { return m_clearColor; }

	GLFWwindow *getWindow() const { return m_window; }

	// Attach/detach UI manager (implemented in .cpp to avoid circular include)
	void attachUiManager(std::unique_ptr<Iui> ui);
	// UI manager attachment through addon system
	// void attachUIManager(std::unique_ptr<Mui> ui);
	void detachUIManager();

	void setUiInitialised(bool v) { m_uiInitialised = v; }
	bool isUiInitialised() const { return m_uiInitialised; }

	// Global engine access
	static BlotEngine *getEngine() { return s_instance; }
	static void setEngine(BlotEngine *engine) { s_instance = engine; }

  private:
	std::string m_appName = "Blot App";
	float m_appVersion = 0.1f;
	uint64_t m_frameCount = 0;

	AppSettings m_settings;
	std::unique_ptr<IApp> m_app;
	std::unique_ptr<MEcs> m_ecsManager;
	std::unique_ptr<MAddon> m_addonManager;
	std::unique_ptr<Iui> m_uiManager;
	std::unique_ptr<MRendering> m_renderingManager;
	std::unique_ptr<MCanvas> m_canvasManager;
	std::unique_ptr<MSettings> m_settingsManager;

	blot::WindowSettings m_windowSettings;
	GLFWwindow *m_window;
	bool m_debugMode = false;
	bool m_vsync = true;
	int m_targetFps = 60;
	int m_currentFps = 0;
	glm::vec4 m_clearColor{0.4f, 0.4f, 0.4f, 1.0f};
	// Tracks whether Mui::init() has been called (addon may call it).
	bool m_uiInitialised = false;

	// Global engine instance
	static BlotEngine *s_instance;
};
} // namespace blot
