#pragma once

#include <functional>
#include <imgui.h>
#include <imgui_node_editor.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "windows/Window.h"
#include "ecs/MEcs.h"

namespace blot {

class NodeEditorWindow : public Window {
  public:
	NodeEditorWindow(const std::string &title = "Node Editor",
					 Flags flags = Flags::None);
	virtual ~NodeEditorWindow();

	void setECSManager(std::shared_ptr<MEcs> ecs);
	void renderContents() override;

  private:
	std::shared_ptr<MEcs> m_ecs;

	// NodeEditor context
	ax::NodeEditor::EditorContext *m_editorContext;

	// Helper methods
	void renderNodeCreationButtons();
	void renderNodes();
	void renderConnections();
	void handleNodeCreation();
	void handleConnections();
	void handleNodeDeletion();

	// Node editor state
	int m_nextNodeId = 1;
	std::unordered_map<int,
					   std::unordered_map<std::string, ax::NodeEditor::PinId>>
		m_nodeParamPins;

	// Initialize NodeEditor context
	void initializeNodeEditor();
	void cleanupNodeEditor();
};

} // namespace blot
