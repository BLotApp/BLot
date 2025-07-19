#pragma once
#include "core/core.h"

class GradientDemoApp : public blot::IApp {
public:
    GradientDemoApp() {
        window().width  = 800;
        window().height = 600;
        window().title  = "Blend2D Gradient Demo";
    }

    void setup()  override;
    void update(float) override;
    void draw()   override;
}; 