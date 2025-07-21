#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "core/IManager.h"
#include "core/ISettings.h"

namespace blot {
class BlotEngine;
class AddonBase;
class MAddon : public IManager, public ISettings {
  public:
	explicit MAddon(BlotEngine *engine);
	~MAddon();
	void init() override {}
	void shutdown() override {}

	// Addon registration and management
	void registerAddon(std::shared_ptr<AddonBase> addon);
	void unregisterAddon(const std::string &name);
	std::shared_ptr<AddonBase> getAddon(const std::string &name) const;

	// Addon lifecycle management
	bool initAll();
	void setupAll();
	void updateAll(float deltaTime);
	void drawAll();
	void cleanupAll();

	// Default addon initialization
	void initDefaultAddons();
	void loadDefaultAddons();

	// Addon discovery and loading
	void scanAddonDirectory(const std::string &directory);
	bool loadAddon(const std::string &path);
	void reloadAddon(const std::string &name);
	void reloadAllAddons();

	// Addon state management
	void enableAddon(const std::string &name);
	void disableAddon(const std::string &name);
	bool isAddonEnabled(const std::string &name) const;

	// Addon information
	std::vector<std::string> getAddonNames() const;
	std::vector<std::shared_ptr<AddonBase>> getEnabledAddons() const;
	std::vector<std::shared_ptr<AddonBase>> getAllAddons() const;

	// Dependency resolution
	bool resolveDependencies();
	std::vector<std::string>
	getAddonDependencies(const std::string &name) const;

	// Addon configuration
	void saveAddonConfig();
	void loadAddonConfig();

	// Utility functions
	void setAddonDirectory(const std::string &directory);
	const std::string &getAddonDirectory() const { return m_addonDirectory; }

	// Event system
	void addGlobalEventListener(const std::string &event,
								std::function<void()> callback);
	void triggerGlobalEvent(const std::string &event);

	// ISettings interface
	json getSettings() const override;
	void setSettings(const json &settings) override;

  private:
	// Addon storage
	std::unordered_map<std::string, std::shared_ptr<AddonBase>> m_addons;
	std::vector<std::string> m_addonOrder; // Order for initialization

	// Configuration
	std::string m_addonDirectory;
	std::unordered_map<std::string, bool>
		m_addonConfig; // enabled/disabled state

	// Event system
	std::unordered_map<std::string, std::vector<std::function<void()>>>
		m_globalEventListeners;

	// Helper functions
	bool checkDependencies(const std::string &addonName);
	void sortAddonsByDependencies();
	std::vector<std::string> getCircularDependencies() const;

	blot::BlotEngine *m_engine = nullptr;
};
} // namespace blot
