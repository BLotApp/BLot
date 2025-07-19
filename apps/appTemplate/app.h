#pragma once
#include "core/IApp.h"
#include "core/BlotEngine.h"
#include <memory>
#include <spdlog/spdlog.h>

namespace blot { class BlotEngine; class IApp; }

class AppTemplate : public blot::IApp {
public:
    AppTemplate() {
        window().width = 1024;
        window().height = 768;
        window().title = "App Template";
    }
    void setup() override {
        spdlog::info("App Template setup!");
        if (auto eng = getEngine()) {
            eng->setClearColor(0.4f, 0.4f, 0.4f, 1.0f);
        }
    }
    void update(float) override {}
    void draw() override {}
}; 
