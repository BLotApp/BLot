#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace blot {
namespace ecs {

enum class CNodeType {
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

struct CNodePin {
	std::string name;
	std::string type; // "float", "vec2", "color", etc.
	bool isInput = true;
	bool isOutput = false;
	float defaultValue = 0.0f;
	std::string description;
};

struct CNodeConnection {
	int fromNodeId;
	std::string fromPin;
	int toNodeId;
	std::string toPin;
};

struct CNodeComponent {
	int nodeId = 0;
	CNodeType type = CNodeType::Custom;
	std::string name;
	std::string category;

	// Node position in editor
	float posX = 0.0f;
	float posY = 0.0f;

	// Pins (inputs/outputs)
	std::vector<CNodePin> pins;

	// Connections
	std::vector<CNodeConnection> connections;

	// Node properties (serializable)
	std::unordered_map<std::string, float> properties;

	// Visual properties
	bool isSelected = false;
	bool isVisible = true;

	// Default constructor
	CNodeComponent() : nodeId(0), type(CNodeType::Custom), name("") {
		setupDefaultPins();
	}
	// Constructor for easy node creation
	CNodeComponent(CNodeType type, const std::string &name = "")
		: nodeId(0), type(type), name(name) {
		setupDefaultPins();
	}
	// Helper methods to get input/output pins
	std::vector<CNodePin> getInputs() const {
		std::vector<CNodePin> inputs;
		for (const auto &pin : pins) {
			if (pin.isInput)
				inputs.push_back(pin);
		}
		return inputs;
	}
	std::vector<CNodePin> getOutputs() const {
		std::vector<CNodePin> outputs;
		for (const auto &pin : pins) {
			if (pin.isOutput)
				outputs.push_back(pin);
		}
		return outputs;
	}

  private:
	void setupDefaultPins() {
		switch (type) {
		case CNodeType::Circle:
			pins = {
				{"x", "float", true, false, 0.0f, "X position"},
				{"y", "float", true, false, 0.0f, "Y position"},
				{"radius", "float", true, false, 50.0f, "Radius"},
				{"color", "color", true, false, 0.0f, "Fill color"},
				{"stroke", "color", true, false, 0.0f, "Stroke color"},
				{"strokeWidth", "float", true, false, 1.0f, "Stroke width"}};
			break;
		case CNodeType::Rectangle:
			pins = {
				{"x", "float", true, false, 0.0f, "X position"},
				{"y", "float", true, false, 0.0f, "Y position"},
				{"width", "float", true, false, 100.0f, "Width"},
				{"height", "float", true, false, 100.0f, "Height"},
				{"color", "color", true, false, 0.0f, "Fill color"},
				{"stroke", "color", true, false, 0.0f, "Stroke color"},
				{"strokeWidth", "float", true, false, 1.0f, "Stroke width"}};
			break;
		case CNodeType::Add:
			pins = {{"a", "float", true, false, 0.0f, "First value"},
					{"b", "float", true, false, 0.0f, "Second value"},
					{"result", "float", false, true, 0.0f, "Sum"}};
			break;
		case CNodeType::Multiply:
			pins = {{"a", "float", true, false, 1.0f, "First value"},
					{"b", "float", true, false, 1.0f, "Second value"},
					{"result", "float", false, true, 1.0f, "Product"}};
			break;
		default:
			// Add some default pins for unknown types
			pins = {{"input", "float", true, false, 0.0f, "Input"},
					{"output", "float", false, true, 0.0f, "Output"}};
			break;
		}
	}
};

} // namespace ecs
} // namespace blot
