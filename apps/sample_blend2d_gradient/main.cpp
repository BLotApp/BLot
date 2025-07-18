#include "core/core.h"
#include "rendering/rendering.h"
#include <memory>
#include <spdlog/spdlog.h>
#include "app_entry.h" // for createApp()

int main() {
    // Create the app instance
    std::unique_ptr<IApp> appInstance = createApp();
    // Create the engine
    BlotEngine engine(std::move(appInstance));
    // Switch to Blend2D renderer if available
    auto renderer = createRenderer(RendererType::Blend2D);
    if (renderer) {
        spdlog::info("Using Blend2D renderer for this app.");
        engine.getRenderingManager()->getMainRenderer()->shutdown();
        engine.getRenderingManager()->setMainRenderer(renderer);
        engine.getRenderingManager()->getMainRenderer()->initialize(1280, 720); // Example size
    } else {
        spdlog::warn("Blend2D renderer not available, using default renderer.");
    }
    engine.run();
    return 0;
} 