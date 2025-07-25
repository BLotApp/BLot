#include "MAddon.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>
#include <unordered_set>
// Removed direct bxImGui include; addons are now registered by applications.
#include "AddonRegistry.h"
#include "IAddon.h"
#include "ISettings.h"
#include "json.h"

MAddon::MAddon(BlotEngine *engine)
	: m_engine(engine), m_addonDirectory("addons") {}

MAddon::~MAddon() { cleanupAll(); }

void MAddon::unregisterAddon(const std::string &name) {
	auto it = m_addons.find(name);
	if (it != m_addons.end()) {
		it->second->cleanup();
		m_addons.erase(it);

		// Remove from order
		auto orderIt =
			std::find(m_addonOrder.begin(), m_addonOrder.end(), name);
		if (orderIt != m_addonOrder.end()) {
			m_addonOrder.erase(orderIt);
		}

		spdlog::info("Unregistered addon: {}", name);
	}
}

std::shared_ptr<IAddon> MAddon::getAddon(const std::string &name) const {
	auto it = m_addons.find(name);
	if (it != m_addons.end()) {
		return it->second;
	}
	return nullptr;
}

bool MAddon::initAll() {
	spdlog::info("Initializing all addons...");

	// Sort addons by dependencies
	sortAddonsByDependencies();

	// Check for circular dependencies
	auto circular = getCircularDependencies();
	if (!circular.empty()) {
		spdlog::error("Circular dependencies detected:");
		for (const auto &dep : circular) {
			spdlog::error("  {}", dep);
		}
		return false;
	}

	// Initialize addons in order
	for (const auto &name : m_addonOrder) {
		auto addon = m_addons[name];
		if (addon && addon->isEnabled()) {
			spdlog::info("Initializing addon: {}", name);
			addon->blotInit();
			if (!addon->isInitialized()) {
				spdlog::error("Failed to initialize addon: {}", name);
				return false;
			}
		}
	}

	spdlog::info("All addons initialized successfully");
	return true;
}

void MAddon::setupAll() {
	for (const auto &name : m_addonOrder) {
		auto addon = m_addons[name];
		if (addon && addon->isEnabled() && addon->isInitialized()) {
			addon->blotSetup();
		}
	}
}

void MAddon::updateAll(float deltaTime) {
	for (const auto &name : m_addonOrder) {
		auto addon = m_addons[name];
		if (addon && addon->isEnabled() && addon->isInitialized()) {
			addon->blotUpdate(deltaTime);
		}
	}
}

void MAddon::drawAll() {
	for (const auto &name : m_addonOrder) {
		auto addon = m_addons[name];
		if (addon && addon->isEnabled() && addon->isInitialized()) {
			addon->blotDraw();
		}
	}
}

void MAddon::cleanupAll() {
	spdlog::info("Cleaning up all addons...");
	for (auto it = m_addonOrder.rbegin(); it != m_addonOrder.rend(); ++it) {
		auto addon = m_addons[*it];
		if (addon && addon->isInitialized()) {
			addon->blotCleanup();
		}
	}
	spdlog::info("All addons cleaned up");
}

void MAddon::initDefaultAddons() {
	// This would initialize default addons that are always available
	spdlog::info("Initializing default addons...");
}

void MAddon::loadDefaultAddons() {
	// This would load addons that are always enabled by default
	spdlog::info("Loading default addons...");
}

void MAddon::scanAddonDirectory(const std::string &directory) {
	spdlog::info("Scanning addon directory: {}", directory);

	if (!std::filesystem::exists(directory)) {
		spdlog::warn("Addon directory does not exist: {}", directory);
		return;
	}

	for (const auto &entry : std::filesystem::directory_iterator(directory)) {
		if (entry.is_directory()) {
			std::string addonPath = entry.path().string();
			std::string addonName = entry.path().filename().string();

			// Check for addon.json manifest
			std::string manifestPath = addonPath + "/addon.json";
			if (std::filesystem::exists(manifestPath)) {
				spdlog::info("Found addon manifest: {}", manifestPath);
				loadFromManifest(manifestPath);
			}
		}
	}
}

bool MAddon::loadAddon(const std::string &path) {
	spdlog::info("Loading addon from path: {}", path);

	// Try to load from manifest first
	if (std::filesystem::exists(path + "/addon.json")) {
		return loadFromManifest(path + "/addon.json");
	}

	// Try to load as a single file
	if (std::filesystem::exists(path)) {
		// This would implement dynamic library loading
		spdlog::warn("Direct addon loading not implemented yet");
		return false;
	}

	return false;
}

void MAddon::reloadAddon(const std::string &name) {
	spdlog::info("Reloading addon: {}", name);

	auto addon = getAddon(name);
	if (addon) {
		addon->blotCleanup();
		addon->blotInit();
		if (addon->isInitialized()) {
			addon->blotSetup();
		}
	}
}

void MAddon::reloadAllAddons() {
	spdlog::info("Reloading all addons...");
	cleanupAll();
	initAll();
	setupAll();
}

bool MAddon::loadFromManifest(const std::string &path) {
	spdlog::info("Loading addon from manifest: {}", path);

	try {
		std::ifstream f(path);
		if (!f.is_open()) {
			spdlog::error("Could not open manifest file: {}", path);
			return false;
		}

		json manifest;
		f >> manifest;

		if (!manifest.contains("name")) {
			spdlog::error("Manifest missing 'name' field: {}", path);
			return false;
		}

		std::string addonName = manifest["name"];
		spdlog::info("Loading addon: {}", addonName);

		// Try to create the addon using the registry
		auto addon = AddonRegistry::instance().create(addonName);
		if (addon) {
			registerAddon(addon);
			return true;
		} else {
			spdlog::error("Could not create addon: {}", addonName);
			return false;
		}
	} catch (const std::exception &e) {
		spdlog::error("Error loading manifest {}: {}", path, e.what());
		return false;
	}
}

void MAddon::enableAddon(const std::string &name) {
	auto addon = getAddon(name);
	if (addon) {
		addon->setEnabled(true);
		m_addonConfig[name] = true;
		spdlog::info("Enabled addon: {}", name);
	}
}

void MAddon::disableAddon(const std::string &name) {
	auto addon = getAddon(name);
	if (addon) {
		addon->setEnabled(false);
		m_addonConfig[name] = false;
		spdlog::info("Disabled addon: {}", name);
	}
}

bool MAddon::isAddonEnabled(const std::string &name) const {
	auto addon = getAddon(name);
	return addon ? addon->isEnabled() : false;
}

std::vector<std::string> MAddon::getAddonNames() const {
	std::vector<std::string> names;
	for (const auto &pair : m_addons) {
		names.push_back(pair.first);
	}
	return names;
}

std::vector<std::shared_ptr<IAddon>> MAddon::getEnabledAddons() const {
	std::vector<std::shared_ptr<IAddon>> enabled;
	for (const auto &pair : m_addons) {
		if (pair.second && pair.second->isEnabled()) {
			enabled.push_back(pair.second);
		}
	}
	return enabled;
}

std::vector<std::shared_ptr<IAddon>> MAddon::getAllAddons() const {
	std::vector<std::shared_ptr<IAddon>> all;
	for (const auto &pair : m_addons) {
		if (pair.second) {
			all.push_back(pair.second);
		}
	}
	return all;
}

bool MAddon::resolveDependencies() {
	spdlog::info("Resolving addon dependencies...");

	// Check all addon dependencies
	for (const auto &pair : m_addons) {
		if (!checkDependencies(pair.first)) {
			spdlog::error("Dependency resolution failed for addon: {}",
						  pair.first);
			return false;
		}
	}

	spdlog::info("All dependencies resolved successfully");
	return true;
}

std::vector<std::string>
MAddon::getAddonDependencies(const std::string &name) const {
	auto addon = getAddon(name);
	if (addon) {
		return addon->getDependencies();
	}
	return {};
}

void MAddon::saveAddonConfig() {
	spdlog::info("Saving addon configuration...");

	json config;
	for (const auto &pair : m_addons) {
		config[pair.first] = {{"enabled", pair.second->isEnabled()},
							  {"settings", pair.second->getSettings()}};
	}

	// Save to file
	std::ofstream f("addon_config.json");
	if (f.is_open()) {
		f << config.dump(2);
		spdlog::info("Addon configuration saved");
	} else {
		spdlog::error("Could not save addon configuration");
	}
}

void MAddon::loadAddonConfig() {
	spdlog::info("Loading addon configuration...");

	std::ifstream f("addon_config.json");
	if (!f.is_open()) {
		spdlog::warn("No addon configuration file found");
		return;
	}

	try {
		json config;
		f >> config;

		for (const auto &[name, addonConfig] : config.items()) {
			auto addon = getAddon(name);
			if (addon) {
				if (addonConfig.contains("enabled")) {
					addon->setEnabled(addonConfig["enabled"]);
				}
				if (addonConfig.contains("settings")) {
					addon->setSettings(addonConfig["settings"]);
				}
			}
		}

		spdlog::info("Addon configuration loaded");
	} catch (const std::exception &e) {
		spdlog::error("Error loading addon configuration: {}", e.what());
	}
}

void MAddon::setAddonDirectory(const std::string &directory) {
	m_addonDirectory = directory;
}

void MAddon::addGlobalEventListener(const std::string &event,
									std::function<void()> callback) {
	m_globalEventListeners[event].push_back(callback);
}

void MAddon::triggerGlobalEvent(const std::string &event) {
	auto it = m_globalEventListeners.find(event);
	if (it != m_globalEventListeners.end()) {
		for (auto &callback : it->second) {
			callback();
		}
	}
}

json MAddon::getSettings() const {
	json settings;
	settings["addonDirectory"] = m_addonDirectory;
	settings["addonConfig"] = m_addonConfig;
	settings["addonOrder"] = m_addonOrder;

	json addons;
	for (const auto &pair : m_addons) {
		addons[pair.first] = pair.second->getSettings();
	}
	settings["addons"] = addons;

	return settings;
}

void MAddon::setSettings(const json &settings) {
	if (settings.contains("addonDirectory")) {
		m_addonDirectory = settings["addonDirectory"];
	}
	if (settings.contains("addonConfig")) {
		m_addonConfig = settings["addonConfig"];
	}
	if (settings.contains("addonOrder")) {
		m_addonOrder = settings["addonOrder"];
	}
	if (settings.contains("addons")) {
		for (const auto &[name, addonSettings] : settings["addons"].items()) {
			auto addon = getAddon(name);
			if (addon) {
				addon->setSettings(addonSettings);
			}
		}
	}
}

bool MAddon::checkDependencies(const std::string &addonName) {
	auto addon = getAddon(addonName);
	if (!addon) {
		return false;
	}

	for (const auto &dep : addon->getDependencies()) {
		auto depAddon = getAddon(dep);
		if (!depAddon) {
			spdlog::error("Missing dependency '{}' for addon '{}'", dep,
						  addonName);
			return false;
		}
		if (!depAddon->isEnabled()) {
			spdlog::error("Dependency '{}' is disabled for addon '{}'", dep,
						  addonName);
			return false;
		}
	}

	return true;
}

void MAddon::sortAddonsByDependencies() {
	// Simple topological sort implementation
	std::vector<std::string> sorted;
	std::unordered_set<std::string> visited;
	std::unordered_set<std::string> temp;

	std::function<void(const std::string &)> visit =
		[&](const std::string &name) {
			if (temp.find(name) != temp.end()) {
				// Circular dependency detected
				return;
			}
			if (visited.find(name) != visited.end()) {
				return;
			}

			temp.insert(name);

			auto addon = getAddon(name);
			if (addon) {
				for (const auto &dep : addon->getDependencies()) {
					visit(dep);
				}
			}

			temp.erase(name);
			visited.insert(name);
			sorted.push_back(name);
		};

	for (const auto &pair : m_addons) {
		visit(pair.first);
	}

	m_addonOrder = sorted;
}

std::vector<std::string> MAddon::getCircularDependencies() const {
	// This is a simplified implementation
	// A full implementation would use a proper cycle detection algorithm
	std::vector<std::string> circular;

	for (const auto &pair : m_addons) {
		auto addon = pair.second;
		if (addon) {
			for (const auto &dep : addon->getDependencies()) {
				auto depAddon = getAddon(dep);
				if (depAddon) {
					for (const auto &depDep : depAddon->getDependencies()) {
						if (depDep == pair.first) {
							std::string cycle =
								pair.first + " -> " + dep + " -> " + depDep;
							circular.push_back(cycle);
						}
					}
				}
			}
		}
	}

	return circular;
}

void MAddon::registerAddon(std::shared_ptr<IAddon> addon) {
	if (!addon) {
		spdlog::error("Attempted to register null addon");
		return;
	}

	std::string name = addon->getName();
	if (m_addons.find(name) != m_addons.end()) {
		spdlog::warn("Addon '{}' already registered, replacing", name);
	}

	m_addons[name] = addon;
	m_addonOrder.push_back(name);

	// Set default enabled state
	if (m_addonConfig.find(name) == m_addonConfig.end()) {
		m_addonConfig[name] = true;
	}

	spdlog::info("Registered addon: {} (v{})", name, addon->getVersion());
}
