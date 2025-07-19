#include <memory>
#include <spdlog/spdlog.h>
#include "core/U_core.h"
#include "rendering/U_rendering.h"

int main() {
	auto app = std::make_unique<GradientDemoApp>();
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
