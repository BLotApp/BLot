// Third-party
#include <spdlog/spdlog.h>

// Project headers
#include "MWindow.h"
#include "Mui.h"
#include "bxBlend2d.h"
#include "rendering/U_rendering.h"
#include "windows/StrokeWindow.h"
#include "core/AddonRegistry.h"

namespace blot {

// Factory function for Blend2DRenderer
namespace {
std::unique_ptr<IRenderer> createBlend2DRenderer() {
	return std::make_unique<Blend2DRenderer>();
}
} // namespace

bxBlend2d::bxBlend2d() : blot::IAddon("bxBlend2d", "1.0.0") {}

bxBlend2d::~bxBlend2d() { cleanup(); }

bool bxBlend2d::init() {
	spdlog::info("[bxBlend2DAddon] Initializing and registering Blend2D "
				 "renderer factory");
	registerRendererFactory();
	m_initialized = true;
	if (m_blotEngine && m_blotEngine->getUiManager()) {
		auto winMgr = m_blotEngine->getUiManager()->getWindowManager();
		if (winMgr) {
			auto strokeWin =
				std::make_shared<StrokeWindow>("Stroke###StrokeWindow");
			winMgr->createWindow(strokeWin->getTitle(), strokeWin);
		}
	}
	return true;
}

void bxBlend2d::setup() {}
void bxBlend2d::update(float) {}
void bxBlend2d::draw() {}
void bxBlend2d::cleanup() {}

void bxBlend2d::registerRendererFactory() {
	// Register the factory with the renderer registry
	RendererRegistry::instance().registerFactory(RendererType::Blend2D,
												 createBlend2DRenderer);
}

} // namespace blot

// Auto-register the addon with the AddonRegistry
BLOT_REGISTER_ADDON(bxBlend2d)
