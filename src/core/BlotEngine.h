#pragma once
#include <memory>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "core/IApp.h"

// Forward declarations for all managers and IApp
namespace blot {
    class IApp;
    class ECSManager;
    class AddonManager;
    class UIManager;
    class RenderingManager;
    class CanvasManager;
    class SettingsManager;
}

namespace blot {
class BlotEngine {
public:
    BlotEngine(std::unique_ptr<IApp> app);
    ~BlotEngine();
    void run();

    ECSManager* getECSManager() { return m_ecsManager.get(); }
    AddonManager* getAddonManager() { return m_addonManager.get(); }
    UIManager* getUIManager() { return m_uiManager.get(); }
    RenderingManager* getRenderingManager() { return m_renderingManager.get(); }
    CanvasManager* getCanvasManager() { return m_canvasManager.get(); }
    SettingsManager* getSettings() { return m_settingsManager.get(); }

    void setDebugMode(bool enabled) { m_debugMode = enabled; }
    bool getDebugMode() const { return m_debugMode; }
    void toggleDebugMode() { m_debugMode = !m_debugMode; }

    // Background clear colour used each frame (for apps without a Canvas)
    void setClearColor(float r, float g, float b, float a = 1.0f) { m_clearColor = glm::vec4(r, g, b, a); }
    glm::vec4 getClearColor() const { return m_clearColor; }

    GLFWwindow* getWindow() const { return m_window; }
private:
    std::unique_ptr<IApp> m_app;
    std::unique_ptr<ECSManager> m_ecsManager;
    std::unique_ptr<AddonManager> m_addonManager;
    std::unique_ptr<UIManager> m_uiManager;
    std::unique_ptr<RenderingManager> m_renderingManager;
    std::unique_ptr<CanvasManager> m_canvasManager;
    std::unique_ptr<SettingsManager> m_settingsManager;

    WindowSettings m_windowSettings;
    GLFWwindow* m_window;
    bool m_debugMode = false;

    glm::vec4 m_clearColor{0.4f, 0.4f, 0.4f, 1.0f};
};
} // namespace blot
