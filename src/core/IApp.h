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
    // Called before setup(), allows the app to specify window parameters
    virtual void configureWindow(WindowSettings& settings) {}
    virtual void setup(blot::BlotEngine* engine) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void draw() = 0;

    // Framework convenience: the engine pointer is set by BlotEngine before setup()
    void setEngine(blot::BlotEngine* engine) { m_engine = engine; }
    blot::BlotEngine* getEngine() const { return m_engine; }

protected:
    // Convenience accessors for commonly-used managers. These return nullptr if the engine is not yet set.
    blot::ECSManager* getECSManager() const;
    blot::RenderingManager* getRenderingManager() const;
    blot::CanvasManager* getCanvasManager() const;
    blot::UIManager* getUIManager() const;
    blot::AddonManager* getAddonManager() const;
    blot::SettingsManager* getSettingsManager() const;

    blot::BlotEngine* m_engine = nullptr; // Populated automatically by the framework
};
} // namespace blot 