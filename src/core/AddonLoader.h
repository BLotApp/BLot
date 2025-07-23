#pragma once

#include <memory>
#include "core/AddonBase.h"
#include "core/MAddon.h"

namespace blot {

// Forward declarations for addon classes
class bxImGui;
class bxScriptEngine;
class bxCodeEditor;
class bxBlend2DAddon;
class bxMarkdown;
class bxNodeEditor;
class bxOsc;
class bxTemplate;
class bxTestEngine;

/**
 * Centralized addon loading utility
 *
 * This class provides a clean interface for applications to register
 * and load addons without relying on static initialization or
 * individual Registration.cpp files.
 */
class AddonLoader {
  public:
	/**
	 * Register all available addons with the addon manager
	 *
	 * @param addonManager The MAddon instance to register addons with
	 * @param addonNames List of addon names to register (empty = all available)
	 */
	static void
	registerAvailableAddons(MAddon *addonManager,
							const std::vector<std::string> &addonNames = {});

	/**
	 * Register a specific addon by name
	 *
	 * @param addonManager The MAddon instance to register the addon with
	 * @param addonName The name of the addon to register
	 * @return true if the addon was successfully registered
	 */
	static bool registerAddon(MAddon *addonManager,
							  const std::string &addonName);

	/**
	 * Get a list of all available addon names
	 */
	static std::vector<std::string> getAvailableAddonNames();

  private:
	// Factory functions for each addon
	static std::shared_ptr<AddonBase> createbxImGui();
	static std::shared_ptr<AddonBase> createbxScriptEngine();
	static std::shared_ptr<AddonBase> createbxCodeEditor();
	static std::shared_ptr<AddonBase> createbxBlend2DAddon();
	static std::shared_ptr<AddonBase> createbxMarkdown();
	static std::shared_ptr<AddonBase> createbxNodeEditor();
	static std::shared_ptr<AddonBase> createbxOsc();
	static std::shared_ptr<AddonBase> createbxTemplate();
	static std::shared_ptr<AddonBase> createbxTestEngine();
};

} // namespace blot
