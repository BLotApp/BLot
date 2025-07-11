#include "node/NodeEditor.h"
#include <imgui.h>
#include <imgui_node_editor.h>
#include <vector>
#include <string>

namespace ed = ax::NodeEditor;

static bool s_nodeEditorOpen = true;

// Node/Pin/Link IDs
static ed::EditorContext* s_editor = nullptr;
static int s_nextId = 1;
static int nextId() { return s_nextId++; }

struct Node {
    int id;
    std::string name;
    int inputPin;
    int outputPin;
    ImVec4 color;
};

struct Link {
    int id;
    int startPin;
    int endPin;
};

static std::vector<Node> s_nodes;
static std::vector<Link> s_links;

void NodeEditor_ShowWindow(bool* p_open) {
    if (!s_editor) s_editor = ed::CreateEditor();
    if (!p_open || !*p_open) return;
    ImGui::Begin("Node Editor", p_open, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) {
                s_nodes.clear();
                s_links.clear();
            }
            if (ImGui::MenuItem("Exit")) {
                *p_open = false;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Add Node")) {
            if (ImGui::MenuItem("Color Node")) {
                int nid = nextId();
                s_nodes.push_back({nid, "Color", nextId(), nextId(), ImVec4(1,0,0,1)});
            }
            if (ImGui::MenuItem("Value Node")) {
                int nid = nextId();
                s_nodes.push_back({nid, "Value", nextId(), nextId(), ImVec4(0,1,0,1)});
            }
            if (ImGui::MenuItem("Output Node")) {
                int nid = nextId();
                s_nodes.push_back({nid, "Output", nextId(), nextId(), ImVec4(1,1,1,1)});
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ed::SetCurrentEditor(s_editor);
    ed::Begin("My Node Editor");
    // Draw nodes
    for (auto& node : s_nodes) {
        ed::BeginNode(node.id);
        ImGui::Text("%s", node.name.c_str());
        ed::BeginPin(node.inputPin, ed::PinKind::Input);
        ImGui::Text("In");
        ed::EndPin();
        if (node.name == "Color") {
            ImGui::ColorEdit4("##color", (float*)&node.color, ImGuiColorEditFlags_NoInputs);
        } else if (node.name == "Value") {
            ImGui::SliderFloat("##value", &node.color.x, 0.0f, 1.0f);
        }
        ed::BeginPin(node.outputPin, ed::PinKind::Output);
        ImGui::Text("Out");
        ed::EndPin();
        ed::EndNode();
    }
    // Draw links
    for (auto& link : s_links) {
        ed::Link(link.id, link.startPin, link.endPin);
    }
    // Handle new links
    if (ed::BeginCreate()) {
        ed::PinId startPinId, endPinId;
        if (ed::QueryNewLink(&startPinId, &endPinId)) {
            if (startPinId && endPinId && startPinId != endPinId) {
                if (ed::AcceptNewItem()) {
                    s_links.push_back({nextId(), (int)startPinId.Get(), (int)endPinId.Get()});
                }
            }
        }
    }
    ed::EndCreate();
    ed::End();
    ImGui::End();
} 