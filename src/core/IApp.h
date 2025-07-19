#pragma once
#include <string>

namespace blot {
class BlotEngine;
class ECSManager;
class AddonManager;
class UIManager;
class RenderingManager;
class CanvasManager;
class SettingsManager;
}

struct WindowSettings {
    int width = 1280;
    int height = 720;
    std::string title = "Blot App";
    bool fullscreen = false;
};

namespace blot {
class IApp {
public:
    virtual ~IApp() = default;

    // ---------------------------------------------------------------------
    // Framework entry points (called by BlotEngine)  – DO NOT override.
    // ---------------------------------------------------------------------
    void blotSetup(blot::BlotEngine* engine);
    void blotUpdate(float deltaTime);
    void blotDraw();

    // Engine pointer helpers
    void setEngine(blot::BlotEngine* engine) { m_engine = engine; }
    blot::BlotEngine* getEngine() const { return m_engine; }

    // Access window settings (can be modified in constructor before engine creation)
    WindowSettings& window() { return settings_; }
    const WindowSettings& window() const { return settings_; }

protected:
    // ---------------------------------------------------------------------
    // User hooks – override these in your app (openFrameworks-style names)
    // ---------------------------------------------------------------------
    virtual void setup() {}
    virtual void update(float) {}
    virtual void draw() {}

    // Convenience accessors
    blot::ECSManager* getECSManager() const;
    blot::RenderingManager* getRenderingManager() const;
    blot::CanvasManager* getCanvasManager() const;
    blot::UIManager* getUIManager() const;
    blot::AddonManager* getAddonManager() const;
    blot::SettingsManager* getSettingsManager() const;

    blot::BlotEngine* m_engine = nullptr;
    WindowSettings settings_;
};
} // namespace blot 