#pragma once

#include "core/IApp.h"
#include "core/BlotEngine.h"
#include "ecs/ECSManager.h"
#include "core/canvas/CanvasManager.h"
#include "rendering/RenderingManager.h"
#include "ui/UIManager.h"

namespace blot { class BlotEngine; class IApp; }

class SampleUiApp : public blot::IApp {
public:
    SampleUiApp();
    ~SampleUiApp();

    void setup(blot::BlotEngine* engine) override;
    void update(float deltaTime) override;
    void draw() override;
    void configureWindow(WindowSettings& settings) override;

}; 