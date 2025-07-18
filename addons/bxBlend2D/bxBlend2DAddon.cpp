// Third-party
#include <spdlog/spdlog.h>

// Project headers
#include "bxBlend2DAddon.h"
#include "rendering/rendering.h"

namespace blot {

// Factory function for Blend2DRenderer
namespace {
std::unique_ptr<IRenderer> createBlend2DRenderer() {
	return std::make_unique<Blend2DRenderer>();
}
} // namespace

bxBlend2DAddon::bxBlend2DAddon() : AddonBase("bxBlend2D", "1.0.0") {}

bxBlend2DAddon::~bxBlend2DAddon() { cleanup(); }

bool bxBlend2DAddon::init() {
	spdlog::info("[bxBlend2DAddon] Initializing and registering Blend2D "
				 "renderer factory");
	registerRendererFactory();
	m_initialized = true;
	return true;
}

void bxBlend2DAddon::setup() {}
void bxBlend2DAddon::update(float) {}
void bxBlend2DAddon::draw() {}
void bxBlend2DAddon::cleanup() {}

void bxBlend2DAddon::registerRendererFactory() {
	// Register the factory with the renderer registry
	RendererRegistry::instance().registerFactory(RendererType::Blend2D,
												 createBlend2DRenderer);
}

} // namespace blot
