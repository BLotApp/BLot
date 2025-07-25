#include "AddonLoader.h"
#include <spdlog/spdlog.h>
#include "AddonRegistry.h"
#include "MAddon.h"

void blot::AddonLoader::registerAvailableAddons(
	MAddon *addonManager, const std::vector<std::string> &addonNames) {
	if (!addonManager) {
		spdlog::error("[AddonLoader] No addon manager provided");
		return;
	}

	std::vector<std::string> namesToRegister = addonNames;
	if (namesToRegister.empty()) {
		namesToRegister = getAvailableAddonNames();
	}

	for (const auto &name : namesToRegister) {
		if (registerAddon(addonManager, name)) {
			spdlog::info("[AddonLoader] Successfully registered addon: {}",
						 name);
		} else {
			spdlog::warn("[AddonLoader] Failed to register addon: {}", name);
		}
	}
}

bool blot::AddonLoader::registerAddon(MAddon *addonManager,
								const std::string &addonName) {
	if (!addonManager) {
		spdlog::error("[AddonLoader] No addon manager provided");
		return false;
	}

	// Use the AddonRegistry to create the addon
	auto addon = AddonRegistry::instance().create(addonName);
	if (addon) {
		addonManager->registerAddon(addon);
		return true;
	} else {
		spdlog::error("[AddonLoader] Unknown addon: {}", addonName);
		return false;
	}
}

std::vector<std::string> blot::AddonLoader::getAvailableAddonNames() {
	return {"bxImGui",	 "bxScriptEngine", "bxCodeEditor",
			"bxBlend2d", "bxMarkdown",	   "bxNodeEditor",
			"bxOsc",	 "bxTemplate",	   "bxTestEngine"};
} 