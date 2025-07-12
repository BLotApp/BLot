#pragma once

#include "Window.h"
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <imgui.h>
#include <imgui_node_editor.h>

namespace blot {

// Forward declarations for node data structures
struct NodeParam {
    std::string name;
    float value;
};
struct NodeConnection {
    int fromNodeId;
    std::string fromParam;
    int toNodeId;
    std::string toParam;
};
enum class NodeType { Circle, Add, Multiply, Sin, Cos, Grid, Copy };
struct Node {
    int id;
    NodeType type;
    std::vector<NodeParam> params;
    std::vector<NodeConnection> inputs;
};

class NodeEditorWindow : public Window {
public:
    NodeEditorWindow(const std::string& title = "Node Editor", Flags flags = Flags::None);
    virtual ~NodeEditorWindow() = default;

    void setNodes(std::vector<Node>* nodes);
    void setNextNodeId(int* nextNodeId);
    virtual void render() override;

private:
    std::vector<Node>* m_nodes = nullptr;
    int* m_nextNodeId = nullptr;
};

} // namespace blot 