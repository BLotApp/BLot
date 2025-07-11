#include "addons/AddonManager.h"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <unordered_set>

AddonManager::AddonManager()
    : m_addonDirectory("addons")
{
}

AddonManager::~AddonManager() {
    cleanupAll();
}

void AddonManager::registerAddon(std::shared_ptr<AddonBase> addon) {
    if (!addon) return;
    
    std::string name = addon->getName();
    m_addons[name] = addon;
    m_addonOrder.push_back(name);
    
    std::cout << "Registered addon: " << name << std::endl;
}

void AddonManager::unregisterAddon(const std::string& name) {
    auto it = m_addons.find(name);
    if (it != m_addons.end()) {
        it->second->cleanup();
        m_addons.erase(it);
        
        // Remove from order
        auto orderIt = std::find(m_addonOrder.begin(), m_addonOrder.end(), name);
        if (orderIt != m_addonOrder.end()) {
            m_addonOrder.erase(orderIt);
        }
        
        std::cout << "Unregistered addon: " << name << std::endl;
    }
}

std::shared_ptr<AddonBase> AddonManager::getAddon(const std::string& name) const {
    auto it = m_addons.find(name);
    if (it != m_addons.end()) {
        return it->second;
    }
    return nullptr;
}

bool AddonManager::initAll() {
    std::cout << "Initializing all addons..." << std::endl;
    
    // Sort addons by dependencies
    sortAddonsByDependencies();
    
    // Check for circular dependencies
    auto circular = getCircularDependencies();
    if (!circular.empty()) {
        std::cerr << "Circular dependencies detected:" << std::endl;
        for (const auto& dep : circular) {
            std::cerr << "  " << dep << std::endl;
        }
        return false;
    }
    
    // Initialize addons in order
    for (const auto& name : m_addonOrder) {
        auto addon = m_addons[name];
        if (addon && addon->isEnabled()) {
            std::cout << "Initializing addon: " << name << std::endl;
            if (!addon->init()) {
                std::cerr << "Failed to initialize addon: " << name << std::endl;
                return false;
            }
        }
    }
    
    std::cout << "All addons initialized successfully" << std::endl;
    return true;
}

void AddonManager::setupAll() {
    for (const auto& name : m_addonOrder) {
        auto addon = m_addons[name];
        if (addon && addon->isEnabled() && addon->isInitialized()) {
            addon->setup();
        }
    }
}

void AddonManager::updateAll(float deltaTime) {
    for (const auto& name : m_addonOrder) {
        auto addon = m_addons[name];
        if (addon && addon->isEnabled() && addon->isInitialized()) {
            addon->update(deltaTime);
        }
    }
}

void AddonManager::drawAll() {
    for (const auto& name : m_addonOrder) {
        auto addon = m_addons[name];
        if (addon && addon->isEnabled() && addon->isInitialized()) {
            addon->draw();
        }
    }
}

void AddonManager::cleanupAll() {
    for (auto& pair : m_addons) {
        if (pair.second) {
            pair.second->cleanup();
        }
    }
    m_addons.clear();
    m_addonOrder.clear();
}

void AddonManager::scanAddonDirectory(const std::string& directory) {
    m_addonDirectory = directory;
    
    if (!std::filesystem::exists(directory)) {
        std::cout << "Addon directory does not exist: " << directory << std::endl;
        return;
    }
    
    std::cout << "Scanning addon directory: " << directory << std::endl;
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_directory()) {
            std::string addonPath = entry.path().string();
            std::string addonName = entry.path().filename().string();
            
            // Check for addon configuration file
            std::filesystem::path configFile = entry.path() / "addon.json";
            if (std::filesystem::exists(configFile)) {
                std::cout << "Found addon: " << addonName << std::endl;
                // In a real implementation, you would load the addon here
            }
        }
    }
}

bool AddonManager::loadAddon(const std::string& path) {
    // This would dynamically load an addon from a shared library
    // For now, we'll just return true
    std::cout << "Loading addon from: " << path << std::endl;
    return true;
}

void AddonManager::reloadAddon(const std::string& name) {
    auto addon = getAddon(name);
    if (addon) {
        addon->cleanup();
        // In a real implementation, you would reload the addon here
        addon->init();
        addon->setup();
    }
}

void AddonManager::enableAddon(const std::string& name) {
    auto addon = getAddon(name);
    if (addon) {
        addon->setEnabled(true);
        m_addonConfig[name] = true;
    }
}

void AddonManager::disableAddon(const std::string& name) {
    auto addon = getAddon(name);
    if (addon) {
        addon->setEnabled(false);
        m_addonConfig[name] = false;
    }
}

bool AddonManager::isAddonEnabled(const std::string& name) const {
    auto it = m_addons.find(name);
    if (it != m_addons.end()) {
        return it->second->isEnabled();
    }
    return false;
}

std::vector<std::string> AddonManager::getAddonNames() const {
    std::vector<std::string> names;
    for (const auto& pair : m_addons) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::shared_ptr<AddonBase>> AddonManager::getEnabledAddons() const {
    std::vector<std::shared_ptr<AddonBase>> enabled;
    for (const auto& pair : m_addons) {
        if (pair.second->isEnabled()) {
            enabled.push_back(pair.second);
        }
    }
    return enabled;
}

std::vector<std::shared_ptr<AddonBase>> AddonManager::getAllAddons() const {
    std::vector<std::shared_ptr<AddonBase>> all;
    for (const auto& pair : m_addons) {
        all.push_back(pair.second);
    }
    return all;
}

bool AddonManager::resolveDependencies() {
    for (const auto& pair : m_addons) {
        if (!checkDependencies(pair.first)) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> AddonManager::getAddonDependencies(const std::string& name) const {
    auto addon = getAddon(name);
    if (addon) {
        return addon->getDependencies();
    }
    return {};
}

void AddonManager::saveAddonConfig() {
    // Save addon configuration to file
    std::cout << "Saving addon configuration..." << std::endl;
}

void AddonManager::loadAddonConfig() {
    // Load addon configuration from file
    std::cout << "Loading addon configuration..." << std::endl;
}

void AddonManager::setAddonDirectory(const std::string& directory) {
    m_addonDirectory = directory;
}

void AddonManager::addGlobalEventListener(const std::string& event, std::function<void()> callback) {
    m_globalEventListeners[event].push_back(callback);
}

void AddonManager::triggerGlobalEvent(const std::string& event) {
    auto it = m_globalEventListeners.find(event);
    if (it != m_globalEventListeners.end()) {
        for (auto& callback : it->second) {
            callback();
        }
    }
}

bool AddonManager::checkDependencies(const std::string& addonName) {
    auto addon = getAddon(addonName);
    if (!addon) return false;
    
    for (const auto& dep : addon->getDependencies()) {
        auto depAddon = getAddon(dep);
        if (!depAddon || !depAddon->isEnabled()) {
            std::cerr << "Addon " << addonName << " depends on " << dep << " which is not available" << std::endl;
            return false;
        }
    }
    
    return true;
}

void AddonManager::sortAddonsByDependencies() {
    // Simple topological sort for dependencies
    // In a real implementation, you'd want a more sophisticated algorithm
    std::vector<std::string> sorted;
    std::unordered_set<std::string> visited;
    
    for (const auto& name : m_addonOrder) {
        if (visited.find(name) == visited.end()) {
            // Add dependencies first
            auto addon = getAddon(name);
            if (addon) {
                for (const auto& dep : addon->getDependencies()) {
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

std::vector<std::string> AddonManager::getCircularDependencies() const {
    // Simple circular dependency detection
    // In a real implementation, you'd want a more sophisticated algorithm
    std::vector<std::string> circular;
    
    for (const auto& pair : m_addons) {
        const auto& name = pair.first;
        const auto& addon = pair.second;
        
        for (const auto& dep : addon->getDependencies()) {
            auto depAddon = getAddon(dep);
            if (depAddon) {
                for (const auto& depDep : depAddon->getDependencies()) {
                    if (depDep == name) {
                        circular.push_back(name + " -> " + dep + " -> " + name);
                    }
                }
            }
        }
    }
    
    return circular;
} 