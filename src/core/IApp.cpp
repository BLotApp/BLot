#include "core/IApp.h"
#include "core/BlotEngine.h"
#include "core/Iui.h"

namespace blot {

MEcs *IApp::getECSManager() const {
	return m_engine ? m_engine->getECSManager() : nullptr;
}

MRendering *IApp::getRenderingManager() const {
	return m_engine ? m_engine->getRenderingManager() : nullptr;
}

MCanvas *IApp::getCanvasManager() const {
	return m_engine ? m_engine->getCanvasManager() : nullptr;
}

Mui *IApp::getUIManager() const {
	return m_engine ? m_engine->getUIManager() : nullptr;
}

Iui *IApp::getUiManager() const {
	return m_engine ? m_engine->getUiManager() : nullptr;
}

MAddon *IApp::getAddonManager() const {
	return m_engine ? m_engine->getAddonManager() : nullptr;
}

MSettings *IApp::getSettingsManager() const {
	return m_engine ? m_engine->getSettings() : nullptr;
}

// -----------------------------------------------------------------------------
// Template-method public wrappers
// -----------------------------------------------------------------------------

void IApp::blotSetup(BlotEngine *engine) {
	// Store engine pointer
	// (redundant if already set by BlotEngine, butharmless)
	setEngine(engine);

	// Framework-level initialisation
	if (auto ui = getUIManager()) {
		ui->setBlotEngine(engine);
	}

	setup();
}

void IApp::blotUpdate(float deltaTime) {
	// Framework-level update
	if (auto ui = getUIManager()) {
		ui->update();
	}

	// User-level update
	update(deltaTime);
}

void IApp::blotDraw() { draw(); }

// -----------------------------------------------------------------------------
// Convenience helpers
// -----------------------------------------------------------------------------

uint64_t IApp::frameCount() const {
	return m_engine ? m_engine->getFrameCount() : 0;
}

} // namespace blot
