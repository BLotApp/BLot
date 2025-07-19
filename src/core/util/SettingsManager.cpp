#include "core/util/SettingsManager.h"
#include "core/ISettings.h"
#include "core/json.h"
#include "core/util/AppPaths.h"
#include <fstream>
#include <spdlog/spdlog.h>

namespace blot {

SettingsManager::SettingsManager() = default;

bool SettingsManager::saveSettings(const blot::json &settings) {
	std::string settingsFile = AppPaths::getUserSettingsFile();
	std::ofstream out(settingsFile);
	if (out.is_open()) {
		out << settings.dump(2);
		spdlog::info("[Settings] Saved settings to {}", settingsFile);
		return true;
	} else {
		spdlog::error("[Settings] Failed to save settings to {}", settingsFile);
		return false;
	}
}

bool SettingsManager::loadSettings(blot::json &outSettings) {
	std::string settingsFile = AppPaths::getUserSettingsFile();
	std::ifstream in(settingsFile);
	if (!in.is_open()) {
		spdlog::info("[Settings] No settings file found, using defaults");
		outSettings = blot::json{};
		return false;
	}
	try {
		in >> outSettings;
		spdlog::info("[Settings] Loaded settings from {}", settingsFile);
		return true;
	} catch (const std::exception &e) {
		spdlog::error("[Settings] Error loading settings: {}", e.what());
		outSettings = blot::json{};
		return false;
	}
}

blot::json SettingsManager::getSettings() const {
	// If you have internal state, serialize it here. For now, just return an
	// empty JSON.
	blot::json j;
	// Optionally, load from file or internal cache
	return j;
}

void SettingsManager::setSettings(const blot::json &settings) {
	// If you have internal state, restore it here. For now, do nothing.
	// Optionally, save to file or internal cache
}

} // namespace blot
