#pragma once
#include "core/core.h"

class SampleMenubarApp : public blot::IApp {
public:
    SampleMenubarApp() {
        window().width = 1280;
        window().height = 720;
        window().title = "Blot Sample Menubar";
    }

    void setup() override {
        spdlog::info("SampleMenubarApp setup");
        if (auto eng = getEngine()) {
            eng->setClearColor(0.15f, 0.15f, 0.2f, 1.0f);
            if (auto ui = getUIManager()) {
                ui->setupWindows(eng);
                ui->setupWindowCallbacks(eng);
            }
        }
    }
    void update(float) override {}
    void draw() override {
        if (auto ui = getUIManager()) {
            ui->update();
        }
    }
}; 