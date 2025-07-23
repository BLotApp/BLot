#include "app.h"
#include <spdlog/spdlog.h>
#include "addons/bxImGui/src/Mui.h"
#include "core/AddonLoader.h"

void ExampleApp::setup() {
	spdlog::info("Setting up example application...");
	getEngine()->init("Example App", 0.1f);

	// Register addons using the new AddonLoader
	auto addonManager = getEngine()->getAddonManager();
	if (addonManager) {
		// Register specific addons that this app needs
		blot::AddonLoader::registerAvailableAddons(
			addonManager, {"bxImGui", "bxScriptEngine"});
	}

	// Ensure the UI canvas window is visible if available
	if (auto ui = dynamic_cast<blot::Mui *>(getUiManager())) {
		ui->setWindowVisibility("Canvas", true);
	}
}

void ExampleApp::update(float deltaTime) {
	// Update application logic
}

void ExampleApp::draw() {
	// Sketching area
}
