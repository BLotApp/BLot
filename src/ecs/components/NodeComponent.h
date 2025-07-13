#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace blot {
namespace components {

enum class NodeType {
    Circle,
    Rectangle,
    Line,
    Polygon,
    Star,
    Add,
    Multiply,
    Sin,
    Cos,
    Grid,
    Copy,
    Custom
};

struct NodePin {
    std::string name;
    std::string type;  // "float", "vec2", "color", etc.
    bool isInput = true;
    bool isOutput = false;
    float defaultValue = 0.0f;
    std::string description;
};

struct NodeConnection {
    int fromNodeId;
    std::string fromPin;
    int toNodeId;
    std::string toPin;
};

struct NodeComponent {
    int nodeId;
    NodeType type;
    std::string name;
    std::string category;
    
    // Node position in editor
    float posX = 0.0f;
    float posY = 0.0f;
    
    // Pins (inputs/outputs)
    std::vector<NodePin> pins;
    
    // Connections
    std::vector<NodeConnection> connections;
    
    // Node properties (serializable)
    std::unordered_map<std::string, float> properties;
    
    // Visual properties
    bool isSelected = false;
    bool isVisible = true;
    
    // Constructor for easy node creation
    NodeComponent(NodeType type, const std::string& name = "")
        : type(type), name(name) {
        setupDefaultPins();
    }
    
private:
    void setupDefaultPins() {
        switch (type) {
            case NodeType::Circle:
                pins = {
                    {"x", "float", true, false, 0.0f, "X position"},
                    {"y", "float", true, false, 0.0f, "Y position"},
                    {"radius", "float", true, false, 50.0f, "Radius"},
                    {"color", "color", true, false, 0.0f, "Fill color"},
                    {"stroke", "color", true, false, 0.0f, "Stroke color"},
                    {"strokeWidth", "float", true, false, 1.0f, "Stroke width"}
                };
                break;
            case NodeType::Rectangle:
                pins = {
                    {"x", "float", true, false, 0.0f, "X position"},
                    {"y", "float", true, false, 0.0f, "Y position"},
                    {"width", "float", true, false, 100.0f, "Width"},
                    {"height", "float", true, false, 100.0f, "Height"},
                    {"color", "color", true, false, 0.0f, "Fill color"},
                    {"stroke", "color", true, false, 0.0f, "Stroke color"},
                    {"strokeWidth", "float", true, false, 1.0f, "Stroke width"}
                };
                break;
            case NodeType::Add:
                pins = {
                    {"a", "float", true, false, 0.0f, "First value"},
                    {"b", "float", true, false, 0.0f, "Second value"},
                    {"result", "float", false, true, 0.0f, "Sum"}
                };
                break;
            case NodeType::Multiply:
                pins = {
                    {"a", "float", true, false, 1.0f, "First value"},
                    {"b", "float", true, false, 1.0f, "Second value"},
                    {"result", "float", false, true, 1.0f, "Product"}
                };
                break;
            default:
                break;
        }
    }
};

} // namespace components
} // namespace blot 