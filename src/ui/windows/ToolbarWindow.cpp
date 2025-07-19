#include "ToolbarWindow.h"
#include <fstream>
#include <imgui.h>
#include <iostream>
#include "../third_party/IconFontCppHeaders/IconsFontAwesome5.h"
#include "core/json.h"

namespace blot {

ToolbarWindow::ToolbarWindow(const std::string &title, Flags flags)
	: Window(title, flags) {}

void ToolbarWindow::setCurrentTool(int toolType) { m_currentTool = toolType; }

void ToolbarWindow::setFillColor(const ImVec4 &color) { m_fillColor = color; }

void ToolbarWindow::setStrokeColor(const ImVec4 &color) {
	m_strokeColor = color;
}

void ToolbarWindow::setStrokeWidth(float width) { m_strokeWidth = width; }

void ToolbarWindow::setShowSwatches(bool show) { m_showSwatches = show; }

void ToolbarWindow::setShowStrokePalette(bool show) {
	m_showStrokePalette = show;
}

void ToolbarWindow::setToolActive(bool active) {
	m_toolActive = active;
	if (m_onToolStateChanged) {
		m_onToolStateChanged(m_toolActive, m_toolStartPos);
	}
}

void ToolbarWindow::setToolStartPos(const ImVec2 &pos) {
	m_toolStartPos = pos;
	if (m_onToolStateChanged) {
		m_onToolStateChanged(m_toolActive, m_toolStartPos);
	}
}

void ToolbarWindow::setSwatches(const std::vector<ImVec4> &swatches) {
	m_swatches = swatches;
}

void ToolbarWindow::addSwatch(const ImVec4 &color) {
	m_swatches.push_back(color);
}

void ToolbarWindow::removeSwatch(int index) {
	if (index >= 0 && index < m_swatches.size()) {
		m_swatches.erase(m_swatches.begin() + index);
	}
}

void ToolbarWindow::saveSwatches(const std::string &path) {
	saveSwatchesToFile(path);
}

void ToolbarWindow::loadSwatches(const std::string &path) {
	loadSwatchesFromFile(path);
}

void ToolbarWindow::setOnToolChanged(std::function<void(int)> callback) {
	m_onToolChanged = callback;
}

void ToolbarWindow::setOnFillColorChanged(
	std::function<void(const ImVec4 &)> callback) {
	m_onFillColorChanged = callback;
}

void ToolbarWindow::setOnStrokeColorChanged(
	std::function<void(const ImVec4 &)> callback) {
	m_onStrokeColorChanged = callback;
}

void ToolbarWindow::setOnStrokeWidthChanged(
	std::function<void(float)> callback) {
	m_onStrokeWidthChanged = callback;
}

void ToolbarWindow::setOnToolStateChanged(
	std::function<void(bool, const ImVec2 &)> callback) {
	m_onToolStateChanged = callback;
}

void ToolbarWindow::renderContents() {
	// Show menu tip if enabled
	if (m_showMenuTip && !m_toolName.empty()) {
		ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "Tip: %s",
						   m_toolName.c_str());
		ImGui::Separator();
	}
	renderTools();
	ImGui::SameLine();
	renderColors();

	if (m_showSwatches) {
		renderSwatches();
	}

	renderStrokeColorButtons();
}

void ToolbarWindow::renderTools() {
	ImGui::Text("Tools:");
	ImGui::SameLine();

	toolButton(ICON_FA_SQUARE, 0); // Rectangle
	toolButton(ICON_FA_CIRCLE, 1); // Circle
	toolButton(ICON_FA_MINUS, 2);  // Line
	toolButton(ICON_FA_PEN, 3);	   // Pen
}

void ToolbarWindow::renderColors() {
	ImGui::SameLine();
	ImGui::Text("Fill:");
	ImGui::SameLine();

	if (ImGui::ColorEdit4("##FillColor", (float *)&m_fillColor,
						  ImGuiColorEditFlags_NoInputs |
							  ImGuiColorEditFlags_NoLabel)) {
		if (m_onFillColorChanged) {
			m_onFillColorChanged(m_fillColor);
		}
	}

	ImGui::SameLine();
	ImGui::Text("Stroke:");
	ImGui::SameLine();

	if (ImGui::ColorEdit4("##StrokeColor", (float *)&m_strokeColor,
						  ImGuiColorEditFlags_NoInputs |
							  ImGuiColorEditFlags_NoLabel)) {
		if (m_onStrokeColorChanged) {
			m_onStrokeColorChanged(m_strokeColor);
		}
	}

	ImGui::SameLine();
	ImGui::Text("Width:");
	ImGui::SameLine();

	if (ImGui::DragFloat("##StrokeWidth", &m_strokeWidth, 0.1f, 0.0f, 50.0f,
						 "%.1f")) {
		if (m_onStrokeWidthChanged) {
			m_onStrokeWidthChanged(m_strokeWidth);
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Stroke Settings")) {
		// This will be handled by the main app to show/hide the StrokeWindow
		// The button just indicates the user wants to open stroke settings
	}
}

void ToolbarWindow::renderSwatches() {
	ImGui::Separator();
	ImGui::Text("Swatches:");

	ImGui::RadioButton("Fill", &m_activeSwatchType, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Stroke", &m_activeSwatchType, 1);

	if (ImGui::Button("Save"))
		saveSwatchesToFile(m_swatchesFile);
	ImGui::SameLine();
	if (ImGui::Button("Load"))
		loadSwatchesFromFile(m_swatchesFile);

	int swatchesPerRow = 8;
	for (int i = 0; i < m_swatches.size(); ++i) {
		if (i > 0 && i % swatchesPerRow == 0)
			ImGui::NewLine();

		if (ImGui::ColorButton(
				("##swatch" + std::to_string(i)).c_str(), m_swatches[i],
				ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop,
				ImVec2(24, 24))) {
			if (m_activeSwatchType == 0) {
				m_fillColor = m_swatches[i];
				if (m_onFillColorChanged) {
					m_onFillColorChanged(m_fillColor);
				}
			} else {
				m_strokeColor = m_swatches[i];
				if (m_onStrokeColorChanged) {
					m_onStrokeColorChanged(m_strokeColor);
				}
			}
		}

		ImGui::SameLine();
	}
}

void ToolbarWindow::renderStrokeColorButtons() {
	ImGui::Separator();
	ImGui::Text("Stroke Colors:");

	if (ImGui::Button(ICON_FA_TIMES " No Stroke")) {
		m_strokeColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		if (m_onStrokeColorChanged) {
			m_onStrokeColorChanged(m_strokeColor);
		}
	}
	ImGui::SameLine();

	if (ImGui::Button(ICON_FA_CIRCLE " White Stroke")) {
		m_strokeColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		if (m_onStrokeColorChanged) {
			m_onStrokeColorChanged(m_strokeColor);
		}
	}
	ImGui::SameLine();

	if (ImGui::Button(ICON_FA_CIRCLE " Red Stroke")) {
		m_strokeColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		if (m_onStrokeColorChanged) {
			m_onStrokeColorChanged(m_strokeColor);
		}
	}
	ImGui::SameLine();

	if (ImGui::Button(ICON_FA_CIRCLE " Green Stroke")) {
		m_strokeColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
		if (m_onStrokeColorChanged) {
			m_onStrokeColorChanged(m_strokeColor);
		}
	}
	ImGui::SameLine();

	if (ImGui::Button(ICON_FA_CIRCLE " Blue Stroke")) {
		m_strokeColor = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
		if (m_onStrokeColorChanged) {
			m_onStrokeColorChanged(m_strokeColor);
		}
	}
}

void ToolbarWindow::toolButton(const char *icon, int toolType) {
	bool isSelected = (m_currentTool == toolType);
	if (isSelected) {
		ImGui::PushStyleColor(ImGuiCol_Button,
							  ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
	}

	if (ImGui::Button(icon)) {
		m_currentTool = toolType;
		if (m_onToolChanged) {
			m_onToolChanged(m_currentTool);
		}
	}

	if (isSelected) {
		ImGui::PopStyleColor();
	}

	ImGui::SameLine();
}

void ToolbarWindow::saveSwatchesToFile(const std::string &path) {
	try {
		blot::json j;
		j["swatches"] = blot::json::array();

		for (const auto &color : m_swatches) {
			blot::json colorJson;
			colorJson["r"] = color.x;
			colorJson["g"] = color.y;
			colorJson["b"] = color.z;
			colorJson["a"] = color.w;
			j["swatches"].push_back(colorJson);
		}

		std::ofstream file(path);
		file << j.dump(4);
	} catch (const std::exception &e) {
		std::cerr << "Failed to save swatches: " << e.what() << std::endl;
	}
}

void ToolbarWindow::loadSwatchesFromFile(const std::string &path) {
	try {
		std::ifstream file(path);
		if (file.is_open()) {
			blot::json j;
			file >> j;

			m_swatches.clear();
			for (const auto &colorJson : j["swatches"]) {
				ImVec4 color;
				color.x = colorJson["r"];
				color.y = colorJson["g"];
				color.z = colorJson["b"];
				color.w = colorJson["a"];
				m_swatches.push_back(color);
			}
		}
	} catch (const std::exception &e) {
		std::cerr << "Failed to load swatches: " << e.what() << std::endl;
	}
}

} // namespace blot
