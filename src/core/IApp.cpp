#include "core/IApp.h"
#include "core/BlotEngine.h"

namespace blot {

ECSManager *IApp::getECSManager() const {
	return m_engine ? m_engine->getECSManager() : nullptr;
}

RenderingManager *IApp::getRenderingManager() const {
	return m_engine ? m_engine->getRenderingManager() : nullptr;
}

CanvasManager *IApp::getCanvasManager() const {
	return m_engine ? m_engine->getCanvasManager() : nullptr;
}

UIManager *IApp::getUIManager() const {
	return m_engine ? m_engine->getUIManager() : nullptr;
}

AddonManager *IApp::getAddonManager() const {
	return m_engine ? m_engine->getAddonManager() : nullptr;
}

SettingsManager *IApp::getSettingsManager() const {
	return m_engine ? m_engine->getSettings() : nullptr;
}

// -----------------------------------------------------------------------------
// Template-method public wrappers
// -----------------------------------------------------------------------------

void IApp::blotSetup(BlotEngine *engine) {
	// Store engine pointer (redundant if already set by BlotEngine, but
	// harmless)
	setEngine(engine);

	// Framework-level initialisation could live here (profiling, default
	// canvas, etc.)

	setup();
}

void IApp::blotUpdate(float deltaTime) {
	// Framework-level per-frame work could go here
	update(deltaTime);
}

void IApp::blotDraw() {
	// Framework-level pre/post draw code could go here
	draw();
}

} // namespace blot
