#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "Window.h"
#include "core/BlotEngine.h"
#include "core/canvas/Canvas.h"
#include "ecs/components/CEvent.h"
#include "ecs/systems/SEvent.h"

namespace blot {

class MEcs;
class Mui;
class CodeEditorWindow;
class Mui;

class MainMenuBar {
  public:
	MainMenuBar(const std::string &title);
	~MainMenuBar() = default;

	// Set the event system
	void setEventSystem(blot::ecs::SEvent *eventSystem);

	// Set CodeEditorWindow reference for theme state
	void setCodeEditorWindow(std::shared_ptr<blot::CodeEditorWindow> window) {
		m_codeEditorWindow = window;
	}

	// Set Canvas reference for renderer state
	void setCanvas(std::shared_ptr<Canvas> canvas) { m_canvas = canvas; }

	// Set UI Manager reference for ImGui theme
	void setUIManager(Mui *uiManager) { m_uiManager = uiManager; }

	// State setters (for menu state)
	void setActiveCanvasId(uint32_t id) { m_activeCanvasId = id; }
	void setCanvasEntities(
		const std::vector<std::pair<uint32_t, std::string>> &entities) {
		m_canvasEntities = entities;
	}

	void render();

  private:
	std::string m_title;
	// Event system reference
	blot::ecs::SEvent *m_eventSystem = nullptr;

	// CodeEditorWindow reference for theme state
	std::shared_ptr<blot::CodeEditorWindow> m_codeEditorWindow;

	// Canvas reference for renderer state
	std::shared_ptr<Canvas> m_canvas;

	// UI Manager reference for ImGui theme
	Mui *m_uiManager = nullptr;

	// State variables
	uint32_t m_activeCanvasId = 0;
	std::vector<std::pair<uint32_t, std::string>> m_canvasEntities;

	// Helper methods
	void triggerAction(const std::string &actionId);
	bool hasAction(const std::string &actionId) const;
};

} // namespace blot
