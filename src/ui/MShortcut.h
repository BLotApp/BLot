#pragma once

#include <functional>
#include <imgui.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "rendering/U_gladGlfw.h"

// Modifier bitmask
enum ShortcutModifier {
	Mod_None = 0,
	Mod_Ctrl = 1 << 0,
	Mod_Shift = 1 << 1,
	Mod_Alt = 1 << 2,
	Mod_Super = 1 << 3
};

struct Shortcut {
	ImGuiKey key;  // ImGuiKey_*
	int modifiers; // ShortcutModifier bitmask
	std::function<void()> callback;
	std::string description;
};

class MShortcut {
  public:
	void registerShortcut(ImGuiKey key, int modifiers,
						  std::function<void()> callback,
						  const std::string &description);
	void processShortcuts();
	void showHelpOverlay();

	// Utility
	static int getCurrentModifiers();

  private:
	std::vector<Shortcut> m_shortcuts;
	bool m_showHelp = false;
};
