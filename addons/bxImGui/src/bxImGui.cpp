#include "bxImGui.h"
#include <spdlog/spdlog.h>

using namespace blot;

bxImGui::bxImGui() : blot::IAddon("ImGui", "0.1.0") {}

bool bxImGui::init() {
	// For now, just return true - the UI manager will be set up elsewhere
	// TODO: Implement proper UI manager initialization
	spdlog::info("[bxImGui] Initializing ImGui addon");
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
