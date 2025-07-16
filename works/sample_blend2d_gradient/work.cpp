#include <blend2d.h>
#include <memory>
#include "framework/IWork.h"
#include "framework/FrameworkApp.h"
#include "ecs/ECSManager.h"
#include "rendering/Renderer.h"
#include "addons/AddonManager.h"
#include <spdlog/spdlog.h>
#include "work_entry.h"
#include "rendering/Blend2DRenderer.h"

static Blend2DRenderer g_renderer;

class SampleBlend2DGradientWork : public IWork {
public:
    void setup(FrameworkApp*) override {
        // Any one-time setup logic here
        spdlog::info("SampleBlend2DGradientWork setup");
        g_renderer.initialize(800, 600); // Example size
    }
    void update(float) override {}
    void draw() override {
        g_renderer.clear({1, 1, 1, 1}); // White background
        g_renderer.setFillColor({1, 0, 0, 1}); // Red
        g_renderer.setStrokeColor({0, 0, 0, 1}); // Black
        g_renderer.setStrokeWidth(4.0f);
        g_renderer.drawRect(100, 100, 300, 200);
        g_renderer.present();
    }
};

std::unique_ptr<IWork> createWork() {
    return std::make_unique<SampleBlend2DGradientWork>();
} 