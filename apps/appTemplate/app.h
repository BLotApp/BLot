#pragma once
#include "core/core.h"
#include <memory>
#include <iostream>

namespace blot { class BlotEngine; class IApp; }

class AppTemplate : public blot::IApp {
public:
    void setup(blot::BlotEngine* engine) override {
        std::cout << "App Template setup!" << std::endl;
        engine->setClearColor(0.4f, 0.4f, 0.4f, 1.0f); // mid-grey background
    }
    void update(float) override {}
    void draw() override {}
    void configureWindow(WindowSettings& settings) override;
}; 
