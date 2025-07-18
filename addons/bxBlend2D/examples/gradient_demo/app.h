#pragma once
#include "core/IApp.h"
#include <memory>

namespace blot { class BlotEngine; class IApp; }

class GradientDemoApp : public blot::IApp {
public:
    void setup(blot::BlotEngine*) override;
    void update(float) override;
    void draw() override;
    void configureWindow(WindowSettings& settings) override;
}; 