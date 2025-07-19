#pragma once

#include "core/BlotEngine.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Forward declarations
class BlotEngine;
class Canvas;
class Graphics;
class bxScriptEngine;

namespace blot {
class AddonBase {
  public:
	AddonBase(const std::string &name, const std::string &version = "1.0.0");
	virtual ~AddonBase();

	// Addon lifecycle
	virtual bool init() = 0;
	virtual void setup() = 0;
	virtual void update(float deltaTime) = 0;
	virtual void draw() = 0;
	virtual void cleanup() = 0;

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

	// Access to main application components
	void setBlotEngine(BlotEngine *engine) { m_blotEngine = engine; }
	void setCanvas(std::shared_ptr<Canvas> canvas) { m_canvas = canvas; }
	void setGraphics(std::shared_ptr<Graphics> graphics) {
		m_graphics = graphics;
	}
	void setScriptEngine(std::shared_ptr<bxScriptEngine> scriptEngine) {
		m_scriptEngine = scriptEngine;
	}

	// Utility functions for addons
	template <typename T> T *getAddon(const std::string &name);

	// Event system
	void addEventListener(const std::string &event,
						  std::function<void()> callback);
	void removeEventListener(const std::string &event);
	void triggerEvent(const std::string &event);

  protected:
	// Protected access to main components
	BlotEngine *m_blotEngine;
	std::shared_ptr<Canvas> m_canvas;
	std::shared_ptr<Graphics> m_graphics;
	std::shared_ptr<bxScriptEngine> m_scriptEngine;

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

	// Helper functions for addons
	void log(const std::string &message);
	void error(const std::string &message);
	void warning(const std::string &message);
};

// Template implementation
template <typename T> T *AddonBase::getAddon(const std::string &name) {
	// This would be implemented by the AddonManager
	// For now, return nullptr
	return nullptr;
}
} // namespace blot
