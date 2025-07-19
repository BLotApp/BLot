#include "StrokeWindow.h"
#include <algorithm>
#include <cctype>
#include <sstream>

StrokeWindow::StrokeWindow(const std::string &title, blot::Window::Flags flags)
	: blot::Window(title, flags) {
	// Initialize with default values
	resetToDefaults();
	updateDashArrayInput();
}

void StrokeWindow::renderContents() {
	ImGui::BeginGroup();
	// Basic settings section
	renderBasicSettings();
	ImGui::Separator();
	// Advanced settings toggle
	if (ImGui::CollapsingHeader("Advanced Settings",
								ImGuiTreeNodeFlags_DefaultOpen)) {
		renderAdvancedSettings();
	}
	ImGui::EndGroup();
}

void StrokeWindow::renderBasicSettings() {
	ImGui::Text("Basic Stroke Settings");
	ImGui::Spacing();

	// Stroke Width
	float strokeWidth = static_cast<float>(m_strokeWidth);
	if (ImGui::SliderFloat("Width", &strokeWidth, 0.0f, 50.0f, "%.1f")) {
		setStrokeWidth(static_cast<double>(strokeWidth));
	}

	// Stroke Cap
	const char *capItems[] = {"Butt",	  "Square",
							  "Round",	  "Round Reversed",
							  "Triangle", "Triangle Reversed"};
	int currentCap = static_cast<int>(m_strokeCap);
	if (ImGui::Combo("Cap Style", &currentCap, capItems,
					 IM_ARRAYSIZE(capItems))) {
		setStrokeCap(static_cast<BLStrokeCap>(currentCap));
	}

	// Stroke Join
	const char *joinItems[] = {"Miter Clip", "Miter Bevel", "Miter Round",
							   "Bevel", "Round"};
	int currentJoin = static_cast<int>(m_strokeJoin);
	if (ImGui::Combo("Join Style", &currentJoin, joinItems,
					 IM_ARRAYSIZE(joinItems))) {
		setStrokeJoin(static_cast<BLStrokeJoin>(currentJoin));
	}

	// Miter Limit (only show for miter joins)
	if (m_strokeJoin <= BL_STROKE_JOIN_MITER_ROUND) {
		float miterLimit = static_cast<float>(m_miterLimit);
		if (ImGui::SliderFloat("Miter Limit", &miterLimit, 0.1f, 20.0f,
							   "%.1f")) {
			setMiterLimit(static_cast<double>(miterLimit));
		}
	}
}

void StrokeWindow::renderAdvancedSettings() {
	// Dash Pattern
	renderDashArrayEditor();

	ImGui::Spacing();

	// Dash Offset
	float dashOffset = static_cast<float>(m_dashOffset);
	if (ImGui::SliderFloat("Dash Offset", &dashOffset, 0.0f, 100.0f, "%.1f")) {
		setDashOffset(static_cast<double>(dashOffset));
	}

	// Transform Order
	const char *transformItems[] = {"Transform After Stroke",
									"Transform Before Stroke"};
	int currentTransform = static_cast<int>(m_transformOrder);
	if (ImGui::Combo("Transform Order", &currentTransform, transformItems,
					 IM_ARRAYSIZE(transformItems))) {
		setTransformOrder(
			static_cast<BLStrokeTransformOrder>(currentTransform));
	}

	ImGui::Spacing();

	// Reset button
	if (ImGui::Button("Reset to Defaults")) {
		resetToDefaults();
	}
}

void StrokeWindow::renderDashArrayEditor() {
	ImGui::Text("Dash Pattern");

	// Dash presets
	renderDashPresets();

	ImGui::Spacing();

	// Custom dash pattern input
	ImGui::Text("Custom Pattern (comma-separated values):");

	// Color the input red if invalid
	if (!m_dashArrayInputValid) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	}

	char buffer[256];
	strncpy(buffer, m_dashArrayInput.c_str(), sizeof(buffer) - 1);
	buffer[sizeof(buffer) - 1] = '\0';

	if (ImGui::InputText("##DashArray", buffer, sizeof(buffer))) {
		m_dashArrayInput = buffer;
		updateDashArrayFromInput();
	}

	if (!m_dashArrayInputValid) {
		ImGui::PopStyleColor();
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
						   "Invalid dash pattern");
	}

	// Preview current dash pattern
	if (!m_dashArray.empty()) {
		ImGui::Text("Preview:");
		ImGui::SameLine();

		// Draw a simple preview line
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 size(100, 10);
		ImGui::GetWindowDrawList()->AddRect(
			pos, ImVec2(pos.x + size.x, pos.y + size.y),
			ImGui::GetColorU32(ImGuiCol_Text), 0.0f, 0, 2.0f);

		// Draw dash pattern preview
		float x = pos.x;
		bool draw = true;
		for (double dash : m_dashArray) {
			if (draw) {
				ImGui::GetWindowDrawList()->AddLine(
					ImVec2(x, pos.y + size.y / 2),
					ImVec2(x + dash * 2, pos.y + size.y / 2),
					ImGui::GetColorU32(ImGuiCol_Text), 2.0f);
			}
			x += dash * 2;
			draw = !draw;
		}

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + size.y + 5);
	}
}

void StrokeWindow::renderDashPresets() {
	ImGui::Text("Presets:");

	const char *presets[] = {"Solid",	 "Dashed",	  "Dotted",
							 "Dash-Dot", "Long Dash", "Double Dash"};

	for (int i = 0; i < IM_ARRAYSIZE(presets); ++i) {
		if (ImGui::Button(presets[i])) {
			applyDashPreset(presets[i]);
		}
		if (i % 3 != 2 && i < IM_ARRAYSIZE(presets) - 1) {
			ImGui::SameLine();
		}
	}
}

void StrokeWindow::applyDashPreset(const std::string &presetName) {
	if (presetName == "Solid") {
		setDashArray({});
	} else if (presetName == "Dashed") {
		setDashArray({5.0, 5.0});
	} else if (presetName == "Dotted") {
		setDashArray({1.0, 3.0});
	} else if (presetName == "Dash-Dot") {
		setDashArray({5.0, 3.0, 1.0, 3.0});
	} else if (presetName == "Long Dash") {
		setDashArray({10.0, 5.0});
	} else if (presetName == "Double Dash") {
		setDashArray({5.0, 2.0, 5.0, 5.0});
	}
}

void StrokeWindow::updateDashArrayFromInput() {
	m_dashArrayInputValid = true;
	m_dashArray.clear();

	if (m_dashArrayInput.empty()) {
		return;
	}

	std::istringstream iss(m_dashArrayInput);
	std::string token;

	while (std::getline(iss, token, ',')) {
		// Trim whitespace
		token.erase(0, token.find_first_not_of(" \t\r\n"));
		token.erase(token.find_last_not_of(" \t\r\n") + 1);

		if (token.empty())
			continue;

		try {
			double value = std::stod(token);
			if (value < 0) {
				m_dashArrayInputValid = false;
				return;
			}
			m_dashArray.push_back(value);
		} catch (const std::exception &) {
			m_dashArrayInputValid = false;
			return;
		}
	}

	if (m_dashArrayCallback) {
		m_dashArrayCallback(m_dashArray);
	}
}

void StrokeWindow::updateDashArrayInput() {
	std::ostringstream oss;
	for (size_t i = 0; i < m_dashArray.size(); ++i) {
		if (i > 0)
			oss << ", ";
		oss << m_dashArray[i];
	}
	m_dashArrayInput = oss.str();
}

// Setters
void StrokeWindow::setStrokeWidth(double width) {
	m_strokeWidth = width;
	if (m_strokeWidthCallback) {
		m_strokeWidthCallback(width);
	}
}

void StrokeWindow::setStrokeCap(BLStrokeCap cap) {
	m_strokeCap = cap;
	if (m_strokeCapCallback) {
		m_strokeCapCallback(cap);
	}
}

void StrokeWindow::setStrokeJoin(BLStrokeJoin join) {
	m_strokeJoin = join;
	if (m_strokeJoinCallback) {
		m_strokeJoinCallback(join);
	}
}

void StrokeWindow::setMiterLimit(double limit) {
	m_miterLimit = limit;
	if (m_miterLimitCallback) {
		m_miterLimitCallback(limit);
	}
}

void StrokeWindow::setDashArray(const std::vector<double> &dashArray) {
	m_dashArray = dashArray;
	updateDashArrayInput();
	if (m_dashArrayCallback) {
		m_dashArrayCallback(dashArray);
	}
}

void StrokeWindow::setDashOffset(double offset) {
	m_dashOffset = offset;
	if (m_dashOffsetCallback) {
		m_dashOffsetCallback(offset);
	}
}

void StrokeWindow::setTransformOrder(BLStrokeTransformOrder order) {
	m_transformOrder = order;
	if (m_transformOrderCallback) {
		m_transformOrderCallback(order);
	}
}

void StrokeWindow::resetToDefaults() {
	setStrokeWidth(1.0);
	setStrokeCap(BL_STROKE_CAP_BUTT);
	setStrokeJoin(BL_STROKE_JOIN_MITER_CLIP);
	setMiterLimit(4.0);
	setDashArray({});
	setDashOffset(0.0);
	setTransformOrder(BL_STROKE_TRANSFORM_ORDER_AFTER);
}

BLStrokeOptions StrokeWindow::getBlend2DStrokeOptions() const {
	BLStrokeOptions options;

	options.width = m_strokeWidth;
	options.startCap = static_cast<uint8_t>(m_strokeCap);
	options.endCap = static_cast<uint8_t>(m_strokeCap);
	options.join = static_cast<uint8_t>(m_strokeJoin);
	options.miterLimit = m_miterLimit;
	options.dashOffset = m_dashOffset;
	options.transformOrder = static_cast<uint8_t>(m_transformOrder);

	// Set dash array
	options.dashArray.clear();
	for (double dash : m_dashArray) {
		options.dashArray.append(dash);
	}

	return options;
}

// String conversion helpers
std::string StrokeWindow::strokeCapToString(BLStrokeCap cap) const {
	switch (cap) {
	case BL_STROKE_CAP_BUTT:
		return "Butt";
	case BL_STROKE_CAP_SQUARE:
		return "Square";
	case BL_STROKE_CAP_ROUND:
		return "Round";
	case BL_STROKE_CAP_ROUND_REV:
		return "Round Reversed";
	case BL_STROKE_CAP_TRIANGLE:
		return "Triangle";
	case BL_STROKE_CAP_TRIANGLE_REV:
		return "Triangle Reversed";
	default:
		return "Unknown";
	}
}

std::string StrokeWindow::strokeJoinToString(BLStrokeJoin join) const {
	switch (join) {
	case BL_STROKE_JOIN_MITER_CLIP:
		return "Miter Clip";
	case BL_STROKE_JOIN_MITER_BEVEL:
		return "Miter Bevel";
	case BL_STROKE_JOIN_MITER_ROUND:
		return "Miter Round";
	case BL_STROKE_JOIN_BEVEL:
		return "Bevel";
	case BL_STROKE_JOIN_ROUND:
		return "Round";
	default:
		return "Unknown";
	}
}

std::string
StrokeWindow::transformOrderToString(BLStrokeTransformOrder order) const {
	switch (order) {
	case BL_STROKE_TRANSFORM_ORDER_AFTER:
		return "Transform After Stroke";
	case BL_STROKE_TRANSFORM_ORDER_BEFORE:
		return "Transform Before Stroke";
	default:
		return "Unknown";
	}
}
