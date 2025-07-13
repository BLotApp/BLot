#pragma once

#include "Window.h"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <imgui.h>
#include <imgui_node_editor.h>
#include <unordered_map>

// Forward declarations
class ECSManager;

namespace blot {

class NodeEditorWindow : public Window {
public:
    NodeEditorWindow(const std::string& title = "Node Editor", Flags flags = Flags::None);
    virtual ~NodeEditorWindow() = default;

    void setECSManager(std::shared_ptr<ECSManager> ecs);
    virtual void render() override;

private:
    std::shared_ptr<ECSManager> m_ecs;
    
    // Helper methods
    void renderNodeCreationButtons();
    void renderNodes();
    void renderConnections();
    void handleNodeCreation();
    void handleConnections();
    
    // Node editor state
    int m_nextNodeId = 1;
    std::unordered_map<int, std::unordered_map<std::string, ax::NodeEditor::PinId>> m_nodeParamPins;
};

} // namespace blot 