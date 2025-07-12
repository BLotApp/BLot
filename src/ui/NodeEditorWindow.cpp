#include "NodeEditorWindow.h"
#include <imgui.h>
#include <imgui_node_editor.h>
#include <unordered_map>

namespace ed = ax::NodeEditor;

namespace blot {

NodeEditorWindow::NodeEditorWindow(const std::string& title, Flags flags)
    : Window(title, flags) {}

// Window interface implementations
void NodeEditorWindow::show() { m_state.isOpen = true; }
void NodeEditorWindow::hide() { m_state.isOpen = false; }
void NodeEditorWindow::close() { m_state.isOpen = false; }
void NodeEditorWindow::toggle() { m_state.isOpen = !m_state.isOpen; }
bool NodeEditorWindow::isOpen() const { return m_state.isOpen; }
bool NodeEditorWindow::isVisible() const { return m_state.isOpen; }
bool NodeEditorWindow::isFocused() const { return ImGui::IsWindowFocused(); }
bool NodeEditorWindow::isHovered() const { return ImGui::IsWindowHovered(); }
bool NodeEditorWindow::isDragging() const { return false; }
bool NodeEditorWindow::isResizing() const { return false; }
void NodeEditorWindow::setPosition(const ImVec2& pos) { /* TODO: Implement if needed */ }
void NodeEditorWindow::setSize(const ImVec2& size) { /* TODO: Implement if needed */ }
void NodeEditorWindow::setMinSize(const ImVec2& minSize) { /* TODO: Implement if needed */ }
void NodeEditorWindow::setMaxSize(const ImVec2& maxSize) { /* TODO: Implement if needed */ }
void NodeEditorWindow::setFlags(Window::Flags flags) { m_state.flags = static_cast<int>(flags); }
int NodeEditorWindow::getFlags() const { return m_state.flags; }
std::string NodeEditorWindow::getTitle() const { return m_title; }

void NodeEditorWindow::setNodes(std::vector<Node>* nodes) {
    m_nodes = nodes;
}

void NodeEditorWindow::setNextNodeId(int* nextNodeId) {
    m_nextNodeId = nextNodeId;
}

void NodeEditorWindow::render() {
    begin();
    if (isOpen() && m_nodes && m_nextNodeId) {
        ed::Begin("MyNodeEditor");
        // Add node creation buttons
        if (ImGui::Button("Add Circle Node")) {
            Node n;
            n.id = (*m_nextNodeId)++;
            n.type = NodeType::Circle;
            n.params = { {"x", 100}, {"y", 100}, {"radius", 50} };
            m_nodes->push_back(n);
        }
        ImGui::SameLine();
        if (ImGui::Button("Add Add Node")) {
            Node n;
            n.id = (*m_nextNodeId)++;
            n.type = NodeType::Add;
            n.params = { {"a", 0}, {"b", 0}, {"out", 0} };
            m_nodes->push_back(n);
        }
        ImGui::SameLine();
        if (ImGui::Button("Add Multiply Node")) {
            Node n;
            n.id = (*m_nextNodeId)++;
            n.type = NodeType::Multiply;
            n.params = { {"a", 1}, {"b", 1}, {"out", 1} };
            m_nodes->push_back(n);
        }
        if (ImGui::Button("Add Sin Node")) {
            Node n;
            n.id = (*m_nextNodeId)++;
            n.type = NodeType::Sin;
            n.params = { {"in", 0}, {"out", 0} };
            m_nodes->push_back(n);
        }
        ImGui::SameLine();
        if (ImGui::Button("Add Cos Node")) {
            Node n;
            n.id = (*m_nextNodeId)++;
            n.type = NodeType::Cos;
            n.params = { {"in", 0}, {"out", 0} };
            m_nodes->push_back(n);
        }
        ImGui::SameLine();
        if (ImGui::Button("Add Grid Node")) {
            Node n;
            n.id = (*m_nextNodeId)++;
            n.type = NodeType::Grid;
            n.params = { {"x", 100}, {"y", 100}, {"radius", 30}, {"rows", 3}, {"cols", 3}, {"spacing", 60} };
            m_nodes->push_back(n);
        }
        ImGui::SameLine();
        if (ImGui::Button("Add Copy Node")) {
            Node n;
            n.id = (*m_nextNodeId)++;
            n.type = NodeType::Copy;
            n.params = { {"x", 100}, {"y", 100}, {"radius", 30}, {"count", 5}, {"dx", 40}, {"dy", 0} };
            m_nodes->push_back(n);
        }
        // Draw nodes and pins
        std::unordered_map<int, std::unordered_map<std::string, ed::PinId>> nodeParamPins;
        for (auto& node : *m_nodes) {
            ed::BeginNode(node.id);
            ImGui::Text("%s", node.type == NodeType::Circle ? "Circle" : node.type == NodeType::Add ? "Add" : node.type == NodeType::Multiply ? "Multiply" : node.type == NodeType::Sin ? "Sin" : node.type == NodeType::Cos ? "Cos" : node.type == NodeType::Grid ? "Grid" : node.type == NodeType::Copy ? "Copy" : "Other");
            for (auto& param : node.params) {
                ed::PinId pinId = (node.id << 8) | std::hash<std::string>{}(param.name);
                nodeParamPins[node.id][param.name] = pinId;
                // For math nodes, "out" is output, others are input
                bool isOutput = false;
                if ((node.type == NodeType::Add || node.type == NodeType::Multiply || node.type == NodeType::Sin || node.type == NodeType::Cos) && param.name == "out")
                    isOutput = true;
                if ((node.type == NodeType::Circle || node.type == NodeType::Grid || node.type == NodeType::Copy) && param.name == "radius")
                    isOutput = true;
                ed::BeginPin(pinId, isOutput ? ed::PinKind::Output : ed::PinKind::Input);
                ImGui::PushID(&param);
                ImGui::InputFloat(param.name.c_str(), &param.value);
                ImGui::PopID();
                ed::EndPin();
            }
            ed::EndNode();
        }
        // Draw links
        for (const auto& node : *m_nodes) {
            for (const auto& conn : node.inputs) {
                ed::LinkId linkId = (conn.fromNodeId << 16) | conn.toNodeId;
                ed::PinId fromPin = nodeParamPins[conn.fromNodeId][conn.fromParam];
                ed::PinId toPin = nodeParamPins[conn.toNodeId][conn.toParam];
                ed::Link(linkId, fromPin, toPin);
            }
        }
        // Handle new links
        if (ed::BeginCreate()) {
            ed::PinId startPinId, endPinId;
            if (ed::QueryNewLink(&startPinId, &endPinId)) {
                if (startPinId && endPinId && startPinId != endPinId) {
                    // Find nodes/params for pins
                    int fromNode = static_cast<int>(startPinId.Get() >> 8);
                    int toNode = static_cast<int>(endPinId.Get() >> 8);
                    std::string fromParam, toParam;
                    bool fromIsOutput = false, toIsInput = false;
                    for (auto& node : *m_nodes) {
                        if (node.id == fromNode) {
                            for (auto& param : node.params) {
                                if (ax::NodeEditor::PinId((fromNode << 8) | std::hash<std::string>{}(param.name)) == startPinId) {
                                    fromParam = param.name;
                                    if ((node.type == NodeType::Add || node.type == NodeType::Multiply || node.type == NodeType::Sin || node.type == NodeType::Cos) && param.name == "out")
                                        fromIsOutput = true;
                                    if ((node.type == NodeType::Circle || node.type == NodeType::Grid || node.type == NodeType::Copy) && param.name == "radius")
                                        fromIsOutput = true;
                                }
                            }
                        }
                        if (node.id == toNode) {
                            for (auto& param : node.params) {
                                if (ax::NodeEditor::PinId((toNode << 8) | std::hash<std::string>{}(param.name)) == endPinId) {
                                    toParam = param.name;
                                    if (!(node.type == NodeType::Add || node.type == NodeType::Multiply || node.type == NodeType::Sin || node.type == NodeType::Cos) || param.name != "out")
                                        toIsInput = true;
                                }
                            }
                        }
                    }
                    // Only allow output->input
                    if (fromIsOutput && toIsInput) {
                        for (auto& node : *m_nodes) {
                            if (node.id == toNode) {
                                node.inputs.push_back({fromNode, fromParam, toNode, toParam});
                            }
                        }
                        ed::AcceptNewItem();
                    }
                }
            }
        }
        ed::EndCreate();
        ed::End();
    }
    end();
}

} // namespace blot 