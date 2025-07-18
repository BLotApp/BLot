#include "app.h"
#include "core/BlotEngine.h"
#include "app_entry.h"
#include <blend2d.h>
#include <spdlog/spdlog.h>

void SampleBlend2DGradientApp::setup(BlotEngine*) {
    spdlog::info("SampleBlend2DGradientApp setup");
}
void SampleBlend2DGradientApp::update(float) {}
void SampleBlend2DGradientApp::draw() {}

void SampleBlend2DGradientApp::configureWindow(WindowSettings& settings) {
    settings.width = 800;
    settings.height = 600;
    settings.title = "Gradient Demo";
    settings.fullscreen = false;
}

std::unique_ptr<IApp> createApp() {
    return std::make_unique<SampleBlend2DGradientApp>();
} 