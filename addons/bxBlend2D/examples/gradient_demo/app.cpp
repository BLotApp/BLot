#include "app.h"
#include "core/BlotEngine.h"
#include "app_entry.h"
#include <blend2d.h>
#include <spdlog/spdlog.h>

void GradientDemoApp::setup(blot::BlotEngine*) {
    spdlog::info("GradientDemoApp setup");
}
void GradientDemoApp::update(float) {}
void GradientDemoApp::draw() {}

void GradientDemoApp::configureWindow(WindowSettings& settings) {
    settings.width = 800;
    settings.height = 600;
    settings.title = "Blend2D Gradient Demo";
    settings.fullscreen = false;
}

std::unique_ptr<blot::IApp> createApp() {
    return std::make_unique<GradientDemoApp>();
} 