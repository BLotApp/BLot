#include "core/BlotEngine.h"

#include "rendering/U_gladGlfw.h"

#include <glm/glm.hpp>

#include <chrono>
#include <iostream>
#include <thread>

#include "core/AppSettings.h"
#include "core/IApp.h"
#include "core/MAddon.h"
#include "core/U_core.h"
#include "core/util/MSettings.h"
#include "rendering/U_rendering.h"
#include "ui/Mui.h"
#include "ui/U_ui.h"

namespace blot {

BlotEngine::~BlotEngine() = default;

BlotEngine::BlotEngine(std::unique_ptr<IApp> app)
	: BlotEngine(std::move(app), AppSettings{}) {}

// New constructor with settings

BlotEngine::BlotEngine(std::unique_ptr<IApp> app, const AppSettings &settings)
	: m_settings(settings), m_app(std::move(app)),
	  m_addonManager(std::make_unique<MAddon>()),
	  m_ecsManager(std::make_unique<MEcs>()),
	  m_renderingManager(std::make_unique<MRendering>()),
	  m_canvasManager(std::make_unique<MCanvas>(this)), m_uiManager(nullptr),
	  m_settingsManager(std::make_unique<MSettings>()), m_window(nullptr) {
	// Apply settings
	WindowSettings ws = m_settings.window;
	if (m_app) {
		m_app->setEngine(this);
		// If app had modified its own settings.window before passing, merge
		ws = m_app->settings().window;
	}

	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
	}
	GLFWmonitor *monitor = ws.fullscreen ? glfwGetPrimaryMonitor() : nullptr;
	m_window = glfwCreateWindow(ws.width, ws.height, ws.title.c_str(), monitor,
								nullptr);
	if (!m_window) {
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window");
	}
	glfwMakeContextCurrent(m_window);
#if defined(_WIN32) ||                                                         \
	(defined(__linux__) && !defined(__arm__) && !defined(__aarch64__))
	if (!gladLoaderLoadGL()) {
		throw std::runtime_error("Failed to initialize GLAD");
	}
#elif defined(__arm__) || defined(__aarch64__)
	if (!gladLoaderLoadGLES2()) {
		throw std::runtime_error("Failed to initialize GLAD (GLES2)");
	}
#endif

	m_uiManager = std::make_unique<Mui>(m_window);
	m_addonManager->initDefaultAddons();

	// store settings for later if needed
	m_windowSettings = ws;

	// Apply graphics settings
	setVerticalSync(m_settings.graphics.vsync);
	setTargetFrameRate(m_settings.graphics.targetFps);
	setClearColor(
		m_settings.graphics.clearColor.r, m_settings.graphics.clearColor.g,
		m_settings.graphics.clearColor.b, m_settings.graphics.clearColor.a);
	m_debugMode = m_settings.debugMode;
	m_appName = m_settings.appName;
	m_appVersion = m_settings.version;

	if (m_app) {
		m_app->blotSetup(this);
	}
}

void BlotEngine::init(const std::string &appName, float appVersion) {
	m_appName = appName;
	m_appVersion = appVersion;
	if (m_uiManager) {
		m_uiManager->init();
	}
}

void BlotEngine::run() {

	using clock = std::chrono::high_resolution_clock;
	auto lastTime = clock::now();

	while (!glfwWindowShouldClose(m_window)) {
		auto frameStart = clock::now();
		float deltaTime =
			std::chrono::duration<float>(frameStart - lastTime).count();
		if (deltaTime > 0.0f) {
			m_currentFps = static_cast<int>(1.0f / deltaTime);
		}
		lastTime = frameStart;

		++m_frameCount;
		m_app->blotUpdate(deltaTime);

		// Clear window with user-defined clear colour before custom drawing
		glm::vec4 cc = m_clearColor;
		glClearColor(cc.r, cc.g, cc.b, cc.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_app->blotDraw();
		glfwSwapBuffers(m_window);
		glfwPollEvents();

		// Frame rate limiting (if VSync disabled or monitor faster than target)
		if (m_targetFps > 0) {
			float targetFrame = 1.0f / static_cast<float>(m_targetFps);
			auto frameEnd = clock::now();
			float frameDuration =
				std::chrono::duration<float>(frameEnd - frameStart).count();
			if (frameDuration < targetFrame) {
				std::this_thread::sleep_for(
					std::chrono::duration<float>(targetFrame - frameDuration));
			}
		}
	}
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

// -------------------- VSync & Frame Rate -----------------

void BlotEngine::setVerticalSync(bool enabled) {
	m_vsync = enabled;
	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(enabled ? 1 : 0);
}

void BlotEngine::setTargetFrameRate(int fps) {
	if (fps <= 0) {
		m_targetFps = 0; // uncapped
	} else {
		m_targetFps = fps;
	}
}

} // namespace blot
