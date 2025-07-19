#include <glad/gl.h>
#include <glm/glm.hpp>
#include "core/BlotEngine.h"
#include "core/core.h"
#include "core/IApp.h"
#include "ui/ui.h"
#include "rendering/rendering.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <GLFW/glfw3.h>
#include "core/AddonManager.h"
#include "ui/UIManager.h"
#include "core/util/SettingsManager.h"

namespace blot {

BlotEngine::~BlotEngine() = default;

BlotEngine::BlotEngine(std::unique_ptr<IApp> app)
    : m_app(std::move(app))
    , m_addonManager(std::make_unique<AddonManager>())
    , m_ecsManager(std::make_unique<ECSManager>())
    , m_renderingManager(std::make_unique<RenderingManager>())
    , m_canvasManager(std::make_unique<CanvasManager>(this))
    , m_uiManager(nullptr)
    , m_settingsManager(std::make_unique<SettingsManager>())
    , m_window(nullptr)
{
    WindowSettings ws;
    if (m_app) {
        ws = m_app->window();
    }
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    GLFWmonitor* monitor = ws.fullscreen ? glfwGetPrimaryMonitor() : nullptr;
    m_window = glfwCreateWindow(
        ws.width,
        ws.height,
        ws.title.c_str(),
        monitor,
        nullptr
    );
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(m_window);
#if defined(_WIN32) || (defined(__linux__) && !defined(__arm__) && !defined(__aarch64__))
    if (!gladLoaderLoadGL()) {
        throw std::runtime_error("Failed to initialize GLAD");
    }
#elif defined(__arm__) || defined(__aarch64__)
    if (!gladLoaderLoadGLES2()) {
        throw std::runtime_error("Failed to initialize GLAD (GLES2)");
    }
#endif
    // store settings for later if needed
    m_windowSettings = ws;

    m_uiManager = std::make_unique<UIManager>(m_window);
    m_addonManager->initDefaultAddons();

    if (m_app) {
        m_app->blotSetup(this);
    }
}

void BlotEngine::run() {
    while (!glfwWindowShouldClose(m_window)) {
        float deltaTime = 1.0f / 60.0f; // Placeholder, should compute real delta
        m_app->blotUpdate(deltaTime);

        // Clear window with user-defined clear colour before custom drawing
        glm::vec4 cc = m_clearColor;
        glClearColor(cc.r, cc.g, cc.b, cc.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_app->blotDraw();
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

} // namespace blot 