#include "core/AddonManager.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>
#include <unordered_set>
#include "core/AddonBase.h"
#include "core/ISettings.h"
#include "core/json.h"

blot::AddonManager::AddonManager() : m_addonDirectory("addons") {}

blot::AddonManager::~AddonManager() { cleanupAll(); }

void blot::AddonManager::registerAddon(std::shared_ptr<blot::AddonBase> addon) {
	if (!addon)
		return;

	std::string name = addon->getName();
	m_addons[name] = addon;
	m_addonOrder.push_back(name);

	spdlog::info("Registered addon: {}", name);
}

void blot::AddonManager::unregisterAddon(const std::string &name) {
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

std::shared_ptr<blot::AddonBase>
blot::AddonManager::getAddon(const std::string &name) const {
	auto it = m_addons.find(name);
	if (it != m_addons.end()) {
		return it->second;
	}
	return nullptr;
}

bool blot::AddonManager::initAll() {
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
			if (!addon->init()) {
				spdlog::error("Failed to initialize addon: {}", name);
				return false;
			}
		}
	}

	spdlog::info("All addons initialized successfully");
	return true;
}

void blot::AddonManager::setupAll() {
	for (const auto &name : m_addonOrder) {
		auto addon = m_addons[name];
		if (addon && addon->isEnabled() && addon->isInitialized()) {
			addon->setup();
		}
	}
}

void blot::AddonManager::updateAll(float deltaTime) {
	for (const auto &name : m_addonOrder) {
		auto addon = m_addons[name];
		if (addon && addon->isEnabled() && addon->isInitialized()) {
			addon->update(deltaTime);
		}
	}
}

void blot::AddonManager::drawAll() {
	for (const auto &name : m_addonOrder) {
		auto addon = m_addons[name];
		if (addon && addon->isEnabled() && addon->isInitialized()) {
			addon->draw();
		}
	}
}

void blot::AddonManager::cleanupAll() {
	for (auto &pair : m_addons) {
		if (pair.second) {
			pair.second->cleanup();
		}
	}
	m_addons.clear();
	m_addonOrder.clear();
}

void blot::AddonManager::initDefaultAddons() {
	spdlog::info("Initializing default addons...");

	// Set up addon directory
	setAddonDirectory("addons");
	scanAddonDirectory("addons");

	// Load default addons
	loadDefaultAddons();

	// Initialize all addons
	if (!initAll()) {
		spdlog::error("Failed to initialize addons");
	}
}

void blot::AddonManager::loadDefaultAddons() {
	// In a real implementation, you would dynamically load addons
	// For now, we'll just register some example addons
	spdlog::info("Loading default addons...");

	// Example: Register GUI addon
	// auto guiAddon = std::make_shared<bxGui>();
	// registerAddon(guiAddon);

	// Example: Register OSC addon
	// auto oscAddon = std::make_shared<bxOsc>();
	// registerAddon(oscAddon);
}

// Helper struct for metadata
struct AddonMetadata {
	std::string name;
	std::vector<std::string> dependencies;
};

static AddonMetadata loadAddonMetadata(const std::string &jsonPath) {
	AddonMetadata meta;
	std::ifstream in(jsonPath);
	if (!in)
		return meta;
	blot::json j;
	in >> j;
	meta.name = j.value("name", "");
	if (j.contains("dependencies")) {
		for (const auto &dep : j["dependencies"]) {
			meta.dependencies.push_back(dep.get<std::string>());
		}
	}
	return meta;
}

void blot::AddonManager::scanAddonDirectory(const std::string &directory) {
	m_addonDirectory = directory;
	if (!std::filesystem::exists(directory)) {
		spdlog::info("Addon directory does not exist: {}", directory);
		return;
	}
	spdlog::info("Scanning addon directory: {}", directory);
	m_addonOrder.clear();
	std::unordered_map<std::string, AddonMetadata> allMeta;
	for (const auto &entry : std::filesystem::directory_iterator(directory)) {
		if (entry.is_directory()) {
			std::string addonPath = entry.path().string();
			std::string addonName = entry.path().filename().string();
			std::filesystem::path configFile = entry.path() / "addon.json";
			if (std::filesystem::exists(configFile)) {
				auto meta = loadAddonMetadata(configFile.string());
				if (!meta.name.empty()) {
					allMeta[meta.name] = meta;
				} else {
					spdlog::warn("Addon at '{}' has no name in addon.json",
								 addonPath);
				}
			} else {
				spdlog::warn("No addon.json found for addon '{}', skipping.",
							 addonName);
			}
		}
	}
	// Topological sort for dependency order with error handling
	std::vector<std::string> order;
	std::unordered_set<std::string> visited, visiting;
	bool hasError = false;
	std::function<void(const std::string &)> visit =
		[&](const std::string &name) {
			if (visited.count(name))
				return;
			if (visiting.count(name)) {
				spdlog::error(
					"Circular dependency detected involving addon: {}", name);
				hasError = true;
				return;
			}
			visiting.insert(name);
			auto it = allMeta.find(name);
			if (it != allMeta.end()) {
				for (const auto &dep : it->second.dependencies) {
					if (allMeta.find(dep) == allMeta.end()) {
						spdlog::error(
							"Missing dependency: '{}' required by addon '{}'",
							dep, name);
						hasError = true;
					} else {
						visit(dep);
					}
				}
			}
			order.push_back(name);
			visited.insert(name);
			visiting.erase(name);
		};
	for (const auto &[name, meta] : allMeta) {
		visit(name);
	}
	if (hasError) {
		spdlog::error("Dependency resolution failed due to missing or circular "
					  "dependencies. Addon loading order may be incomplete.");
	}
	m_addonOrder = order;
}

bool blot::AddonManager::loadAddon(const std::string &path) {
	// This would dynamically load an addon from a shared library
	// For now, we'll just return true
	spdlog::info("Loading addon from: {}", path);
	return true;
}

void blot::AddonManager::reloadAddon(const std::string &name) {
	auto addon = getAddon(name);
	if (addon) {
		addon->cleanup();
		// In a real implementation, you would reload the addon here
		addon->init();
		addon->setup();
	}
}

void blot::AddonManager::reloadAllAddons() {
	spdlog::info("Reloading all addons...");

	// Cleanup all addons
	cleanupAll();

	// Re-scan the addon directory
	scanAddonDirectory(m_addonDirectory);

	// Re-initialize all addons
	if (!initAll()) {
		spdlog::error("Failed to reload addons");
	}

	spdlog::info("All addons reloaded");
}

void blot::AddonManager::enableAddon(const std::string &name) {
	auto addon = getAddon(name);
	if (addon) {
		addon->setEnabled(true);
		m_addonConfig[name] = true;
	}
}

void blot::AddonManager::disableAddon(const std::string &name) {
	auto addon = getAddon(name);
	if (addon) {
		addon->setEnabled(false);
		m_addonConfig[name] = false;
	}
}

bool blot::AddonManager::isAddonEnabled(const std::string &name) const {
	auto it = m_addons.find(name);
	if (it != m_addons.end()) {
		return it->second->isEnabled();
	}
	return false;
}

std::vector<std::string> blot::AddonManager::getAddonNames() const {
	std::vector<std::string> names;
	for (const auto &pair : m_addons) {
		names.push_back(pair.first);
	}
	return names;
}

std::vector<std::shared_ptr<blot::AddonBase>>
blot::AddonManager::getEnabledAddons() const {
	std::vector<std::shared_ptr<blot::AddonBase>> enabled;
	for (const auto &pair : m_addons) {
		if (pair.second->isEnabled()) {
			enabled.push_back(pair.second);
		}
	}
	return enabled;
}

std::vector<std::shared_ptr<blot::AddonBase>>
blot::AddonManager::getAllAddons() const {
	std::vector<std::shared_ptr<blot::AddonBase>> all;
	for (const auto &pair : m_addons) {
		all.push_back(pair.second);
	}
	return all;
}

bool blot::AddonManager::resolveDependencies() {
	for (const auto &pair : m_addons) {
		if (!checkDependencies(pair.first)) {
			return false;
		}
	}
	return true;
}

std::vector<std::string>
blot::AddonManager::getAddonDependencies(const std::string &name) const {
	auto addon = getAddon(name);
	if (addon) {
		return addon->getDependencies();
	}
	return {};
}

void blot::AddonManager::saveAddonConfig() {
	// Save addon configuration to file
	spdlog::info("Saving addon configuration...");
}

void blot::AddonManager::loadAddonConfig() {
	// Load addon configuration from file
	spdlog::info("Loading addon configuration...");
}

void blot::AddonManager::setAddonDirectory(const std::string &directory) {
	m_addonDirectory = directory;
}

void blot::AddonManager::addGlobalEventListener(
	const std::string &event, std::function<void()> callback) {
	m_globalEventListeners[event].push_back(callback);
}

void blot::AddonManager::triggerGlobalEvent(const std::string &event) {
	auto it = m_globalEventListeners.find(event);
	if (it != m_globalEventListeners.end()) {
		for (auto &callback : it->second) {
			callback();
		}
	}
}

bool blot::AddonManager::checkDependencies(const std::string &addonName) {
	auto addon = getAddon(addonName);
	if (!addon)
		return false;

	for (const auto &dep : addon->getDependencies()) {
		auto depAddon = getAddon(dep);
		if (!depAddon || !depAddon->isEnabled()) {
			spdlog::error("Addon {} depends on {} which is not available",
						  addonName, dep);
			return false;
		}
	}

	return true;
}

void blot::AddonManager::sortAddonsByDependencies() {
	// Simple topological sort for dependencies
	// In a real implementation, you'd want a more sophisticated algorithm
	std::vector<std::string> sorted;
	std::unordered_set<std::string> visited;

	for (const auto &name : m_addonOrder) {
		if (visited.find(name) == visited.end()) {
			// Add dependencies first
			auto addon = getAddon(name);
			if (addon) {
				for (const auto &dep : addon->getDependencies()) {
					if (visited.find(dep) == visited.end()) {
						sorted.push_back(dep);
						visited.insert(dep);
					}
				}
			}
			sorted.push_back(name);
			visited.insert(name);
		}
	}

	m_addonOrder = sorted;
}

std::vector<std::string> blot::AddonManager::getCircularDependencies() const {
	// Simple circular dependency detection
	// In a real implementation, you'd want a more sophisticated algorithm
	std::vector<std::string> circular;

	for (const auto &pair : m_addons) {
		const auto &name = pair.first;
		const auto &addon = pair.second;

		for (const auto &dep : addon->getDependencies()) {
			auto depAddon = getAddon(dep);
			if (depAddon) {
				for (const auto &depDep : depAddon->getDependencies()) {
					if (depDep == name) {
						circular.push_back(name + " -> " + dep + " -> " + name);
					}
				}
			}
		}
	}

	return circular;
}

blot::json blot::AddonManager::getSettings() const {
	blot::json j;
	j["addonDirectory"] = m_addonDirectory;
	j["addonConfig"] = m_addonConfig;
	j["addonOrder"] = m_addonOrder;
	// Optionally, save more addon state as needed
	return j;
}

void blot::AddonManager::setSettings(const blot::json &settings) {
	if (settings.contains("addonDirectory"))
		m_addonDirectory = settings["addonDirectory"];
	if (settings.contains("addonConfig"))
		m_addonConfig = settings["addonConfig"]
							.get<std::unordered_map<std::string, bool>>();
	if (settings.contains("addonOrder"))
		m_addonOrder = settings["addonOrder"].get<std::vector<std::string>>();
	// Optionally, restore more addon state as needed
}
