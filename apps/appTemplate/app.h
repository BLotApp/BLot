#pragma once
#include "core/core.h"
#include <memory>
#include <iostream>

namespace blot { class BlotEngine; class IApp; }

class AppTemplate : public blot::IApp {
public:
    void setup(blot::BlotEngine*) override {
        std::cout << "App Template setup!" << std::endl;
    }
    void update(float) override {}
    void draw() override {}
    void configureWindow(WindowSettings& settings) override;
}; 
