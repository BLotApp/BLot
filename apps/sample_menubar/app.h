#pragma once
#include "core/BlotEngine.h"
#include <iostream>

class SampleMenubarApp : public blot::IApp {
public:
    SampleMenubarApp() {
        window().width = 1280;
        window().height = 720;
        window().title = "Blot Sample Menubar";
    }

    void setup() override {
        std::cout << "SampleMenubarApp setup" << std::endl;
        if (auto eng = getEngine()) {
            eng->setClearColor(0.15f, 0.15f, 0.2f, 1.0f);
        }
    }
    void update(float) override {}
    void draw() override {
        if (auto ui = getUIManager()) {
            ui->update();
        }
    }
}; 