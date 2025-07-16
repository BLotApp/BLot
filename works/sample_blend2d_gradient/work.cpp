#include <blend2d.h>
#include <memory>
#include "framework/IWork.h"
#include "framework/FrameworkApp.h"
#include "ecs/ECSManager.h"
#include "rendering/Renderer.h"
#include "addons/AddonManager.h"
#include <spdlog/spdlog.h>
#include "work_entry.h"

class SampleBlend2DGradientWork : public IWork {
public:
    void setup(FrameworkApp*) override {
        // Any one-time setup logic here
        spdlog::info("SampleBlend2DGradientWork setup");
    }
    void update(float) override {}
    void draw() override {
        // For demonstration, just log draw
        spdlog::info("Sample image drawn to window");
        // You can add Blend2D drawing logic here if you want
    }
};

std::unique_ptr<IWork> createWork() {
    return std::make_unique<SampleBlend2DGradientWork>();
} 