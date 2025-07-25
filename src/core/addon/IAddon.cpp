#include "IAddon.h"
#include <iostream>
#include <spdlog/spdlog.h>
#include "BlotEngine.h"

namespace blot {

IAddon::IAddon(const std::string &name, const std::string &version)
	: m_name(name), m_version(version), m_description(""), m_author(""),
	  m_license("MIT"), m_enabled(true), m_initialized(false), m_time(0.0f) {}

// Destructor is defaulted in header

void IAddon::addDependency(const std::string &addonName) {
	m_dependencies.push_back(addonName);
}

void IAddon::addEventListener(const std::string &event,
							  std::function<void()> callback) {
	m_eventListeners[event].push_back(callback);
}

void IAddon::removeEventListener(const std::string &event) {
	auto it = m_eventListeners.find(event);
	if (it != m_eventListeners.end()) {
		m_eventListeners.erase(it);
	}
}

void IAddon::triggerEvent(const std::string &event) {
	auto it = m_eventListeners.find(event);
	if (it != m_eventListeners.end()) {
		for (auto &callback : it->second) {
			callback();
		}
	}
}

void IAddon::log(const std::string &message) {
	spdlog::info("[Addon {}] {}", m_name, message);
}

void IAddon::error(const std::string &message) {
	spdlog::error("[Addon {} ERROR] {}", m_name, message);
}

void IAddon::warning(const std::string &message) {
	spdlog::warn("[Addon {} WARNING] {}", m_name, message);
}

// Parameter system implementation
void IAddon::setParameter(const std::string &name, float value) {
	m_parameters[name] = value;

	// Trigger callback if registered
	auto it = m_parameterCallbacks.find(name);
	if (it != m_parameterCallbacks.end()) {
		it->second(value);
	}

	this->log("Parameter " + name + " set to " + std::to_string(value));
}

float IAddon::getParameter(const std::string &name) const {
	auto it = m_parameters.find(name);
	if (it != m_parameters.end()) {
		return it->second;
	}
	return 0.0f;
}

void IAddon::onParameterChanged(const std::string &name,
								std::function<void(float)> callback) {
	m_parameterCallbacks[name] = callback;
}

void IAddon::setParameter(const std::string &name, const std::string &value) {
	m_stringParameters[name] = value;
	this->log("String parameter " + name + " set to " + value);
}

std::string IAddon::getStringParameter(const std::string &name) const {
	auto it = m_stringParameters.find(name);
	if (it != m_stringParameters.end()) {
		return it->second;
	}
	return "";
}

// ISettings implementation
json IAddon::getSettings() const {
	json settings;
	settings["name"] = m_name;
	settings["version"] = m_version;
	settings["description"] = m_description;
	settings["author"] = m_author;
	settings["license"] = m_license;
	settings["enabled"] = m_enabled;
	settings["initialized"] = m_initialized;
	settings["time"] = m_time;
	settings["dependencies"] = m_dependencies;
	settings["parameters"] = m_parameters;
	settings["stringParameters"] = m_stringParameters;
	settings["customSettings"] = m_settings;
	return settings;
}

void IAddon::setSettings(const json &settings) {
	if (settings.contains("name")) m_name = settings["name"];
	if (settings.contains("version")) m_version = settings["version"];
	if (settings.contains("description")) m_description = settings["description"];
	if (settings.contains("author")) m_author = settings["author"];
	if (settings.contains("license")) m_license = settings["license"];
	if (settings.contains("enabled")) m_enabled = settings["enabled"];
	if (settings.contains("time")) m_time = settings["time"];
	if (settings.contains("dependencies")) m_dependencies = settings["dependencies"];
	if (settings.contains("parameters")) m_parameters = settings["parameters"];
	if (settings.contains("stringParameters")) m_stringParameters = settings["stringParameters"];
	if (settings.contains("customSettings")) m_settings = settings["customSettings"];
}

// -----------------------------------------------------------------------------
// Template-method public wrappers
// -----------------------------------------------------------------------------

void IAddon::blotInit() {
	// Framework-level initialization
	this->log("Initializing addon");

	// Call user hook
	m_initialized = this->init();

	if (m_initialized) {
		this->log("Addon initialized successfully");
	} else {
		this->error("Addon initialization failed");
	}
}

void IAddon::blotSetup() {
	// Framework-level setup
	this->log("Setting up addon");

	// Call user hook
	this->setup();
}

void IAddon::blotUpdate(float deltaTime) {
	// Framework-level update
	m_time += deltaTime;

	// Call user hook
	this->update(deltaTime);
}

void IAddon::blotDraw() {
	// Framework-level draw
	// Call user hook
	this->draw();
}

void IAddon::blotCleanup() {
	// Framework-level cleanup
	this->log("Cleaning up addon");

	// Call user hook
	this->cleanup();

	m_initialized = false;
}

BlotEngine *IAddon::getEngine() const { return BlotEngine::getEngine(); }

} // namespace blot 