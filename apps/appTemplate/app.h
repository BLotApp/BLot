#pragma once
#include "core/core.h"
#include <memory>
#include <iostream>

class AppTemplate : public IApp {
public:
    void setup(BlotEngine*) override {
        std::cout << "App Template setup!" << std::endl;
    }
    void update(float) override {}
    void draw() override {}
    void configureWindow(WindowSettings& settings) override;
}; 
