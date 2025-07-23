#include "bxImGui.h"
#include <spdlog/spdlog.h>

using namespace blot;

bxImGui::bxImGui() : blot::IAddon("ImGui", "0.1.0") {}

bool bxImGui::init() {
	spdlog::info("[bxImGui] Initializing ImGui addon");

	auto engine = getEngine();
	if (!engine) {
		spdlog::error("[bxImGui] No engine instance available");
		return false;
	}

	if (engine->getUIManager()) {
		spdlog::info(
			"[bxImGui] UI manager already present – skipping creation");
		m_ui = engine->getUIManager();
		return true;
	}

	// Create UI manager tied to engine window
	auto uiPtr = std::make_unique<Mui>(engine->getWindow());
	m_ui = uiPtr.get();
	engine->attachUIManager(std::move(uiPtr));

	// Call Mui::init() immediately so first frame is safe
	m_ui->init();
	engine->setUiInitialised(true);

	spdlog::info("[bxImGui] ImGui addon initialised");

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
