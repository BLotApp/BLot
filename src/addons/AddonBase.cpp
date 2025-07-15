#include "addons/AddonBase.h"
#include <iostream>
#include <spdlog/spdlog.h>

AddonBase::AddonBase(const std::string& name, const std::string& version)
    : m_name(name)
    , m_version(version)
    , m_description("")
    , m_author("")
    , m_license("MIT")
    , m_enabled(true)
    , m_initialized(false)
    , m_blotApp(nullptr)
{
}

AddonBase::~AddonBase() {
    cleanup();
}

void AddonBase::addDependency(const std::string& addonName) {
    m_dependencies.push_back(addonName);
}

void AddonBase::addEventListener(const std::string& event, std::function<void()> callback) {
    m_eventListeners[event].push_back(callback);
}

void AddonBase::removeEventListener(const std::string& event) {
    auto it = m_eventListeners.find(event);
    if (it != m_eventListeners.end()) {
        m_eventListeners.erase(it);
    }
}

void AddonBase::triggerEvent(const std::string& event) {
    auto it = m_eventListeners.find(event);
    if (it != m_eventListeners.end()) {
        for (auto& callback : it->second) {
            callback();
        }
    }
}

void AddonBase::log(const std::string& message) {
    spdlog::info("[Addon {}] {}", m_name, message);
}

void AddonBase::error(const std::string& message) {
    spdlog::error("[Addon {} ERROR] {}", m_name, message);
}

void AddonBase::warning(const std::string& message) {
    spdlog::warn("[Addon {} WARNING] {}", m_name, message);
}

void AddonBase::cleanup() {
    // Default implementation does nothing
} 