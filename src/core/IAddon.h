#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Forward declarations
class Canvas;
class Graphics;
class bxScriptEngine;

namespace blot {

class IAddon {
  public:
	IAddon(const std::string &name, const std::string &version = "1.0.0");
	virtual ~IAddon() = default;

	// ----------------------------------------
	// Framework entry points DO NOT override.
	// ----------------------------------------
	void blotInit();
	void blotSetup();
	void blotUpdate(float deltaTime);
	void blotDraw();
	void blotCleanup();

	// Addon information
	const std::string &getName() const { return m_name; }
	const std::string &getVersion() const { return m_version; }
	const std::string &getDescription() const { return m_description; }
	const std::string &getAuthor() const { return m_author; }
	const std::string &getLicense() const { return m_license; }

	// Addon state
	bool isEnabled() const { return m_enabled; }
	void setEnabled(bool enabled) { m_enabled = enabled; }
	bool isInitialized() const { return m_initialized; }

	// Dependencies
	void addDependency(const std::string &addonName);
	const std::vector<std::string> &getDependencies() const {
		return m_dependencies;
	}

	// Configuration
	void setDescription(const std::string &description) {
		m_description = description;
	}
	void setAuthor(const std::string &author) { m_author = author; }
	void setLicense(const std::string &license) { m_license = license; }



	// Utility functions for addons
	template <typename T> T *getAddon(const std::string &name);

	// Event system
	void addEventListener(const std::string &event,
						  std::function<void()> callback);
	void removeEventListener(const std::string &event);
	void triggerEvent(const std::string &event);

	// Parameter system (common to many addons)
	void setParameter(const std::string &name, float value);
	float getParameter(const std::string &name) const;
	void onParameterChanged(const std::string &name,
							std::function<void(float)> callback);
	void setParameter(const std::string &name, const std::string &value);
	std::string getStringParameter(const std::string &name) const;

	// Time tracking (common to many addons)
	float getTime() const { return m_time; }
	void resetTime() { m_time = 0.0f; }

  public:
	// ----------------------------------------
	// User hooks – override these in your addon
	// ----------------------------------------
	virtual bool init() { return true; }
	virtual void setup() {}
	virtual void update(float deltaTime) { (void)deltaTime; }
	virtual void draw() {}
	virtual void cleanup() {}

  protected:
	// Addon state
	std::string m_name;
	std::string m_version;
	std::string m_description;
	std::string m_author;
	std::string m_license;
	bool m_enabled;
	bool m_initialized;

	// Dependencies
	std::vector<std::string> m_dependencies;

	// Event system
	std::unordered_map<std::string, std::vector<std::function<void()>>>
		m_eventListeners;

	// Common addon functionality
	float m_time;
	std::unordered_map<std::string, float> m_parameters;
	std::unordered_map<std::string, std::string> m_stringParameters;
	std::unordered_map<std::string, std::function<void(float)>> m_parameterCallbacks;

	// Helper functions for addons
	void log(const std::string &message);
	void error(const std::string &message);
	void warning(const std::string &message);
};

// Template implementation
template <typename T> T *IAddon::getAddon(const std::string &name) {
	// This would be implemented by the Addon Manager
	// For now, return nullptr
	return nullptr;
}

} // namespace blot 