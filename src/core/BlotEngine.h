#pragma once
#include "rendering/gladGlfw.h"
#include <glm/glm.hpp>
#include <memory>
#include "core/WindowSettings.h"
#include "core/core.h"

// Forward declarations for all managers and IApp
namespace blot {
class IApp;
class ECSManager;
class AddonManager;
class UIManager;
class RenderingManager;
class CanvasManager;
class SettingsManager;
} // namespace blot

namespace blot {
class BlotEngine {
  public:
	BlotEngine(std::unique_ptr<IApp> app);
	~BlotEngine();
	void init(const std::string &appName, float appVersion);
	void run();
	void setAppName(const std::string &name) { m_appName = name; }
	void setAppVersion(float version) { m_appVersion = version; }
	const std::string &getAppName() const { return m_appName; }
	float getAppVersion() const { return m_appVersion; }

	ECSManager *getECSManager() { return m_ecsManager.get(); }
	AddonManager *getAddonManager() { return m_addonManager.get(); }
	UIManager *getUIManager() { return m_uiManager.get(); }
	RenderingManager *getRenderingManager() { return m_renderingManager.get(); }
	CanvasManager *getCanvasManager() { return m_canvasManager.get(); }
	SettingsManager *getSettings() { return m_settingsManager.get(); }

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

  private:
	std::string m_appName = "Blot App";
	float m_appVersion = 0.1f;
	std::unique_ptr<IApp> m_app;
	std::unique_ptr<ECSManager> m_ecsManager;
	std::unique_ptr<AddonManager> m_addonManager;
	std::unique_ptr<UIManager> m_uiManager;
	std::unique_ptr<RenderingManager> m_renderingManager;
	std::unique_ptr<CanvasManager> m_canvasManager;
	std::unique_ptr<SettingsManager> m_settingsManager;

	blot::WindowSettings m_windowSettings;
	GLFWwindow *m_window;
	bool m_debugMode = false;
	bool m_vsync = true;
	int m_targetFps = 60;
	int m_currentFps = 0;

	glm::vec4 m_clearColor{0.4f, 0.4f, 0.4f, 1.0f};
	uint64_t m_frameCount = 0;
};
} // namespace blot
