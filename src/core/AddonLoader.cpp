#include "core/AddonLoader.h"
#include <spdlog/spdlog.h>

// Include addon headers
#include "addons/bxBlend2D/bxBlend2DAddon.h"
#include "addons/bxCodeEditor/src/bxCodeEditor.h"
#include "addons/bxImGui/src/bxImGui.h"
#include "addons/bxMarkdown/bxMarkdown.h"
#include "addons/bxNodeEditor/bxNodeEditor.h"
#include "addons/bxOsc/bxOsc.h"
#include "addons/bxScriptEngine/bxScriptEngine.h"
#include "addons/bxTemplate/bxTemplate.h"
#include "addons/bxTestEngine/bxTestEngine.h"

namespace blot {

void AddonLoader::registerAvailableAddons(
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

bool AddonLoader::registerAddon(MAddon *addonManager,
								const std::string &addonName) {
	if (!addonManager) {
		spdlog::error("[AddonLoader] No addon manager provided");
		return false;
	}

	std::shared_ptr<AddonBase> addon;

	// Create the appropriate addon based on name
	if (addonName == "bxImGui") {
		addon = createbxImGui();
	} else if (addonName == "bxScriptEngine") {
		addon = createbxScriptEngine();
	} else if (addonName == "bxCodeEditor") {
		addon = createbxCodeEditor();
	} else if (addonName == "bxBlend2D") {
		addon = createbxBlend2DAddon();
	} else if (addonName == "bxMarkdown") {
		addon = createbxMarkdown();
	} else if (addonName == "bxNodeEditor") {
		addon = createbxNodeEditor();
	} else if (addonName == "bxOsc") {
		addon = createbxOsc();
	} else if (addonName == "bxTemplate") {
		addon = createbxTemplate();
	} else if (addonName == "bxTestEngine") {
		addon = createbxTestEngine();
	} else {
		spdlog::error("[AddonLoader] Unknown addon: {}", addonName);
		return false;
	}

	if (addon) {
		addonManager->registerAddon(addon);
		return true;
	}

	return false;
}

std::vector<std::string> AddonLoader::getAvailableAddonNames() {
	return {"bxImGui",	 "bxScriptEngine", "bxCodeEditor",
			"bxBlend2D", "bxMarkdown",	   "bxNodeEditor",
			"bxOsc",	 "bxTemplate",	   "bxTestEngine"};
}

// Factory function implementations
std::shared_ptr<AddonBase> AddonLoader::createbxImGui() {
	return std::make_shared<bxImGui>();
}

std::shared_ptr<AddonBase> AddonLoader::createbxScriptEngine() {
	return std::make_shared<bxScriptEngine>();
}

std::shared_ptr<AddonBase> AddonLoader::createbxCodeEditor() {
	return std::make_shared<bxCodeEditor>();
}

std::shared_ptr<AddonBase> AddonLoader::createbxBlend2DAddon() {
	return std::make_shared<bxBlend2DAddon>();
}

std::shared_ptr<AddonBase> AddonLoader::createbxMarkdown() {
	return std::make_shared<bxMarkdown>();
}

std::shared_ptr<AddonBase> AddonLoader::createbxNodeEditor() {
	return std::make_shared<bxNodeEditor>();
}

std::shared_ptr<AddonBase> AddonLoader::createbxOsc() {
	return std::make_shared<bxOsc>();
}

std::shared_ptr<AddonBase> AddonLoader::createbxTemplate() {
	return std::make_shared<bxTemplate>();
}

std::shared_ptr<AddonBase> AddonLoader::createbxTestEngine() {
	return std::make_shared<bxTestEngine>();
}

} // namespace blot
