#include "core/core.h"
#include "rendering/rendering.h"
#include <memory>
#include <spdlog/spdlog.h>
#include "app_entry.h"

int main() {
    auto app = createApp();
    blot::BlotEngine engine(std::move(app));

    auto renderer = createRenderer(RendererType::Blend2D);
    if (renderer) {
        spdlog::info("Using Blend2D renderer for this demo.");
        engine.getRenderingManager()->getMainRenderer()->shutdown();
        engine.getRenderingManager()->setMainRenderer(renderer);
        engine.getRenderingManager()->getMainRenderer()->initialize(1280, 720);
    } else {
        spdlog::warn("Blend2D renderer not available, using default renderer.");
    }

    engine.run();
    return 0;
} 