#include "NodeEditorWindow.h"
#include <imgui.h>
#include <imgui_node_editor.h>
#include <unordered_map>
#include "ecs/ECSManager.h"
#include "ecs/components/NodeComponent.h"
#include "ecs/components/ShapeComponent.h"
#include "ecs/components/StyleComponent.h"
#include <iostream>

namespace ed = ax::NodeEditor;

namespace blot {

NodeEditorWindow::NodeEditorWindow(const std::string& title, Flags flags)
    : Window(title, flags) {}

void NodeEditorWindow::setECSManager(std::shared_ptr<ECSManager> ecs) {
    m_ecs = ecs;
}

void NodeEditorWindow::render() {
    if (!m_ecs) return;
    if (isOpen()) {
        if (ImGui::Begin(getTitle().c_str(), &m_isOpen, getFlags())) {
            ed::Begin("NodeEditor");
            renderNodeCreationButtons();
            renderNodes();
            renderConnections();
            handleConnections();
            ed::End();
        }
        ImGui::End();
    }
}

void NodeEditorWindow::renderNodeCreationButtons() {
    if (ImGui::Button("Add Circle Node")) {
        auto entity = m_ecs->createEntity("CircleNode");
        
        // Add NodeComponent
        auto nodeComp = blot::components::NodeComponent(blot::components::NodeType::Circle, "Circle");
        nodeComp.nodeId = m_nextNodeId++;
        m_ecs->addComponent<blot::components::NodeComponent>(entity, nodeComp);
        
        // Add ShapeComponent
        auto shapeComp = blot::components::Shape();
        shapeComp.type = blot::components::Shape::Type::Ellipse;
        shapeComp.x1 = 100.0f;
        shapeComp.y1 = 100.0f;
        shapeComp.x2 = 150.0f;
        shapeComp.y2 = 150.0f;
        m_ecs->addComponent<blot::components::Shape>(entity, shapeComp);
        
        // Add StyleComponent
        auto styleComp = blot::components::Style();
        styleComp.fillR = 1.0f;
        styleComp.fillG = 0.0f;
        styleComp.fillB = 0.0f;
        styleComp.fillA = 1.0f;
        m_ecs->addComponent<blot::components::Style>(entity, styleComp);
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Add Rectangle Node")) {
        auto entity = m_ecs->createEntity("RectangleNode");
        
        auto nodeComp = blot::components::NodeComponent(blot::components::NodeType::Rectangle, "Rectangle");
        nodeComp.nodeId = m_nextNodeId++;
        m_ecs->addComponent<blot::components::NodeComponent>(entity, nodeComp);
        
        auto shapeComp = blot::components::Shape();
        shapeComp.type = blot::components::Shape::Type::Rectangle;
        shapeComp.x1 = 100.0f;
        shapeComp.y1 = 100.0f;
        shapeComp.x2 = 200.0f;
        shapeComp.y2 = 150.0f;
        m_ecs->addComponent<blot::components::Shape>(entity, shapeComp);
        
        auto styleComp = blot::components::Style();
        styleComp.fillR = 0.0f;
        styleComp.fillG = 1.0f;
        styleComp.fillB = 0.0f;
        styleComp.fillA = 1.0f;
        m_ecs->addComponent<blot::components::Style>(entity, styleComp);
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Add Math Node")) {
        auto entity = m_ecs->createEntity("MathNode");
        
        auto nodeComp = blot::components::NodeComponent(blot::components::NodeType::Add, "Add");
        nodeComp.nodeId = m_nextNodeId++;
        m_ecs->addComponent<blot::components::NodeComponent>(entity, nodeComp);
    }
}

void NodeEditorWindow::renderNodes() {
    auto view = m_ecs->view<blot::components::NodeComponent>();
    
    for (auto entity : view) {
        auto& nodeComp = view.get<blot::components::NodeComponent>(entity);
        
        ed::BeginNode(nodeComp.nodeId);
        
        // Node title
        ImGui::Text("%s", nodeComp.name.c_str());
        
        // Render pins
        for (auto& pin : nodeComp.pins) {
            ed::PinId pinId = (nodeComp.nodeId << 8) | std::hash<std::string>{}(pin.name);
            m_nodeParamPins[nodeComp.nodeId][pin.name] = pinId;
            
            ed::BeginPin(pinId, pin.isOutput ? ed::PinKind::Output : ed::PinKind::Input);
            
            ImGui::PushID(&pin);
            
            // Render pin based on type
            if (pin.type == "float") {
                ImGui::InputFloat(pin.name.c_str(), &pin.defaultValue);
            } else if (pin.type == "color") {
                float color[4] = {pin.defaultValue, pin.defaultValue, pin.defaultValue, 1.0f};
                if (ImGui::ColorEdit4(pin.name.c_str(), color)) {
                    pin.defaultValue = color[0];
                }
            }
            
            ImGui::PopID();
            ed::EndPin();
        }
        
        ed::EndNode();
    }
}

void NodeEditorWindow::renderConnections() {
    auto view = m_ecs->view<blot::components::NodeComponent>();
    
    for (auto entity : view) {
        auto& nodeComp = view.get<blot::components::NodeComponent>(entity);
        
        for (const auto& conn : nodeComp.connections) {
            ed::LinkId linkId = (conn.fromNodeId << 16) | conn.toNodeId;
            
            auto fromPinIt = m_nodeParamPins.find(conn.fromNodeId);
            auto toPinIt = m_nodeParamPins.find(conn.toNodeId);
            
            if (fromPinIt != m_nodeParamPins.end() && toPinIt != m_nodeParamPins.end()) {
                auto fromPinIt2 = fromPinIt->second.find(conn.fromPin);
                auto toPinIt2 = toPinIt->second.find(conn.toPin);
                
                if (fromPinIt2 != fromPinIt->second.end() && toPinIt2 != toPinIt->second.end()) {
                    ed::Link(linkId, fromPinIt2->second, toPinIt2->second);
                }
            }
        }
    }
}

void NodeEditorWindow::handleConnections() {
    if (ed::BeginCreate()) {
        ed::PinId startPinId, endPinId;
        if (ed::QueryNewLink(&startPinId, &endPinId)) {
            if (startPinId && endPinId && startPinId != endPinId) {
                // Extract node and pin info from pin IDs
                int fromNode = static_cast<int>(startPinId.Get() >> 8);
                int toNode = static_cast<int>(endPinId.Get() >> 8);
                
                // Find the pins
                std::string fromPin, toPin;
                bool fromIsOutput = false, toIsInput = false;
                
                auto view = m_ecs->view<blot::components::NodeComponent>();
                for (auto entity : view) {
                    auto& nodeComp = view.get<blot::components::NodeComponent>(entity);
                    
                    if (nodeComp.nodeId == fromNode) {
                        for (auto& pin : nodeComp.pins) {
                            ed::PinId testPin = (fromNode << 8) | std::hash<std::string>{}(pin.name);
                            if (testPin == startPinId) {
                                fromPin = pin.name;
                                fromIsOutput = pin.isOutput;
                                break;
                            }
                        }
                    }
                    
                    if (nodeComp.nodeId == toNode) {
                        for (auto& pin : nodeComp.pins) {
                            ed::PinId testPin = (toNode << 8) | std::hash<std::string>{}(pin.name);
                            if (testPin == endPinId) {
                                toPin = pin.name;
                                toIsInput = pin.isInput;
                                break;
                            }
                        }
                    }
                }
                
                // Only allow output->input connections
                if (fromIsOutput && toIsInput) {
                    // Add connection to target node
                    for (auto entity : view) {
                        auto& nodeComp = view.get<blot::components::NodeComponent>(entity);
                        if (nodeComp.nodeId == toNode) {
                            blot::components::NodeConnection conn;
                            conn.fromNodeId = fromNode;
                            conn.fromPin = fromPin;
                            conn.toNodeId = toNode;
                            conn.toPin = toPin;
                            nodeComp.connections.push_back(conn);
                            break;
                        }
                    }
                    ed::AcceptNewItem();
                }
            }
        }
    }
    ed::EndCreate();
}

} // namespace blot 