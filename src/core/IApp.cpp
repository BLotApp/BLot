#include "core/IApp.h"
#include "core/BlotEngine.h"
#include "core/Iui.h"
#include "core/util/AppPaths.h"

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
	if (auto ui = getUiManager()) {
		ui->setBlotEngine(engine);
	}

	// Allow app to register its UI actions now that ECS exists
	if (m_engine && m_engine->getECSManager()) {
		registerUIActions(m_engine->getECSManager()->getEventSystem());
		// load addons from manifest
		m_engine->getAddonManager()->loadFromManifest(
			AppPaths::getManifestPath());
	}

	setup();
}

void IApp::blotUpdate(float deltaTime) {
	m_deltaTime = deltaTime;
	// Framework-level update
	// 1) Update ECS-related systems (canvas, scripts, generic systems)
	if (auto ecs = getECSManager()) {
		auto renderingMgr = getRenderingManager();
		ecs->updateSystems(renderingMgr, deltaTime);
	}

	// 2) Update UI – this also internally updates the WindowManager
	if (auto ui = getUiManager()) {
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
