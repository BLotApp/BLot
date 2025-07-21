#include "bxImGui.h"
#include <spdlog/spdlog.h>
#include "core/BlotEngine.h"

using namespace blot;

bxImGui::bxImGui() : AddonBase("ImGui", "0.1.0") {}

bool bxImGui::init() {
	auto engine = m_blotEngine; // inherited protected member
	if (!engine) {
		spdlog::error(
			"[bxImGui] BlotEngine pointer not set – cannot initialise UI");
		return false;
	}
	if (engine->getUIManager()) {
		spdlog::info("[bxImGui] UI manager already present – skipping creation");
		m_ui = engine->getUIManager();
		return true;
	}

	// Create UI manager tied to engine window
	auto uiPtr = std::make_unique<Mui>(engine->getWindow());
	m_ui = uiPtr.get();
	engine->attachUIManager(std::move(uiPtr));

	// Initialisation handled via engine.init() later or auto here
	// if (!engine->getUIManager()->getSettings().is_null()) {
	//    /* nothing */
	//}

	// Call Mui::init() immediately so first frame is safe
	m_ui->init();
	engine->setUiInitialised(true);
	return true;
}

void bxImGui::update(float dt) {
	// Deliberately empty: IApp::blotUpdate() already invokes m_ui->update() per
	// frame. Calling it here as well would do the same work twice.
}

void bxImGui::cleanup() {
	if (m_ui) {
		// Shutdown handled by Mui destructor when engine releases it
		m_ui = nullptr;
	}
}
