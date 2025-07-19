#pragma once

#include "core/IManager.h"
#include "core/ISettings.h"
#include "core/json.h"

namespace blot {
class SettingsManager : public IManager, public ISettings {
  public:
	SettingsManager();
	void init() override {}
	void shutdown() override {}
	bool saveSettings(const json &settings);
	bool loadSettings(json &outSettings);
	// ISettings interface
	json getSettings() const override;
	void setSettings(const json &settings) override;
};
} // namespace blot
