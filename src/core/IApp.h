#pragma once
#include <string>

namespace blot { class BlotEngine; }

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
};
} // namespace blot 