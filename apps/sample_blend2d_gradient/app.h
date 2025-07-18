#pragma once
#include "core/IApp.h"
#include <memory>

class BlotEngine;

class SampleBlend2DGradientApp : public IApp {
public:
    void setup(BlotEngine*) override;
    void update(float) override;
    void draw() override;
    void configureWindow(WindowSettings& settings) override;
}; 