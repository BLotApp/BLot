#include "ThemeEditorWindow.h"
#include <fstream>
#include <imgui.h>
#include "../third_party/IconFontCppHeaders/IconsFontAwesome5.h"
#include "Mui.h"
#include "core/json.h"

namespace blot {

ThemeEditorWindow::ThemeEditorWindow(const std::string &title, Flags flags)
	: Window(title, flags) {}

void ThemeEditorWindow::setUIManager(Mui *uiManager) {
	m_uiManager = uiManager;
}

void ThemeEditorWindow::renderContents() {
	if (!m_uiManager)
		return;
	renderThemeEditor();
}

void ThemeEditorWindow::renderThemeEditor() {
	// Theme selection
	ImGui::Text(ICON_FA_PALETTE " Theme Selection");
	ImGui::Separator();

	const char *themes[] = {"Dark", "Light", "Classic", "Corporate", "Dracula"};
	int currentTheme = static_cast<int>(m_uiManager->m_currentTheme);
	if (ImGui::Combo("Theme", &currentTheme, themes, IM_ARRAYSIZE(themes))) {
		m_uiManager->setImGuiTheme(static_cast<Mui::ImGuiTheme>(currentTheme));
		m_uiManager->m_currentTheme =
			static_cast<Mui::ImGuiTheme>(currentTheme);
	}

	ImGui::Spacing();

	// Quick theme buttons
	ImGui::Text(ICON_FA_BOLT " Quick Themes");
	ImGui::Separator();

	if (ImGui::Button(ICON_FA_MOON " Dark Theme")) {
		m_uiManager->setImGuiTheme(Mui::ImGuiTheme::Dark);
		m_uiManager->m_currentTheme = Mui::ImGuiTheme::Dark;
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_SUN " Light Theme")) {
		m_uiManager->setImGuiTheme(Mui::ImGuiTheme::Light);
		m_uiManager->m_currentTheme = Mui::ImGuiTheme::Light;
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_PAINT_BRUSH " Classic Theme")) {
		m_uiManager->setImGuiTheme(Mui::ImGuiTheme::Classic);
		m_uiManager->m_currentTheme = Mui::ImGuiTheme::Classic;
	}

	ImGui::Spacing();

	// Save/Load theme buttons
	ImGui::Text(ICON_FA_SAVE " Theme Management");
	ImGui::Separator();

	if (ImGui::Button(ICON_FA_SAVE " Save Current Theme")) {
		m_uiManager->saveCurrentTheme(m_uiManager->m_lastThemePath);
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_FOLDER_OPEN " Load Theme")) {
		m_uiManager->loadTheme(m_uiManager->m_lastThemePath);
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_RANDOM " Random Theme")) {
		// Generate random colors for fun!
		auto &style = ImGui::GetStyle();
		auto &colors = style.Colors;

		// Random color generator
		auto randomColor = []() {
			return ImVec4(static_cast<float>(rand()) / RAND_MAX,
						  static_cast<float>(rand()) / RAND_MAX,
						  static_cast<float>(rand()) / RAND_MAX, 1.0f);
		};

		// Apply random colors to various ImGui elements
		colors[ImGuiCol_Text] = randomColor();
		colors[ImGuiCol_TextDisabled] = randomColor();
		colors[ImGuiCol_WindowBg] = randomColor();
		colors[ImGuiCol_ChildBg] = randomColor();
		colors[ImGuiCol_PopupBg] = randomColor();
		colors[ImGuiCol_Border] = randomColor();
		colors[ImGuiCol_BorderShadow] = randomColor();
		colors[ImGuiCol_FrameBg] = randomColor();
		colors[ImGuiCol_FrameBgHovered] = randomColor();
		colors[ImGuiCol_FrameBgActive] = randomColor();
		colors[ImGuiCol_TitleBg] = randomColor();
		colors[ImGuiCol_TitleBgActive] = randomColor();
		colors[ImGuiCol_TitleBgCollapsed] = randomColor();
		colors[ImGuiCol_MenuBarBg] = randomColor();
		colors[ImGuiCol_ScrollbarBg] = randomColor();
		colors[ImGuiCol_ScrollbarGrab] = randomColor();
		colors[ImGuiCol_ScrollbarGrabHovered] = randomColor();
		colors[ImGuiCol_ScrollbarGrabActive] = randomColor();
		colors[ImGuiCol_CheckMark] = randomColor();
		colors[ImGuiCol_SliderGrab] = randomColor();
		colors[ImGuiCol_SliderGrabActive] = randomColor();
		colors[ImGuiCol_Button] = randomColor();
		colors[ImGuiCol_ButtonHovered] = randomColor();
		colors[ImGuiCol_ButtonActive] = randomColor();
		colors[ImGuiCol_Header] = randomColor();
		colors[ImGuiCol_HeaderHovered] = randomColor();
		colors[ImGuiCol_HeaderActive] = randomColor();
		colors[ImGuiCol_Separator] = randomColor();
		colors[ImGuiCol_SeparatorHovered] = randomColor();
		colors[ImGuiCol_SeparatorActive] = randomColor();
		colors[ImGuiCol_ResizeGrip] = randomColor();
		colors[ImGuiCol_ResizeGripHovered] = randomColor();
		colors[ImGuiCol_ResizeGripActive] = randomColor();
		colors[ImGuiCol_Tab] = randomColor();
		colors[ImGuiCol_TabHovered] = randomColor();
		colors[ImGuiCol_TabActive] = randomColor();
		colors[ImGuiCol_TabUnfocused] = randomColor();
		colors[ImGuiCol_TabUnfocusedActive] = randomColor();
		colors[ImGuiCol_PlotLines] = randomColor();
		colors[ImGuiCol_PlotLinesHovered] = randomColor();
		colors[ImGuiCol_PlotHistogram] = randomColor();
		colors[ImGuiCol_PlotHistogramHovered] = randomColor();
		colors[ImGuiCol_TextSelectedBg] = randomColor();
		colors[ImGuiCol_DragDropTarget] = randomColor();
		colors[ImGuiCol_NavHighlight] = randomColor();
		colors[ImGuiCol_NavWindowingHighlight] = randomColor();
		colors[ImGuiCol_NavWindowingDimBg] = randomColor();
		colors[ImGuiCol_ModalWindowDimBg] = randomColor();
	}

	// Theme path display
	ImGui::Text("Last Theme Path: %s", m_uiManager->m_lastThemePath.c_str());

	ImGui::Spacing();

	// Color customization
	ImGui::Text(ICON_FA_PALETTE " Color Customization");
	ImGui::Separator();

	ImGuiStyle &style = ImGui::GetStyle();
	ImVec4 *colors = style.Colors;

	// Main colors
	if (ImGui::CollapsingHeader("Main Colors",
								ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::ColorEdit4("Text", (float *)&colors[ImGuiCol_Text]);
		ImGui::ColorEdit4("Text Disabled",
						  (float *)&colors[ImGuiCol_TextDisabled]);
		ImGui::ColorEdit4("Window Background",
						  (float *)&colors[ImGuiCol_WindowBg]);
		ImGui::ColorEdit4("Child Background",
						  (float *)&colors[ImGuiCol_ChildBg]);
		ImGui::ColorEdit4("Popup Background",
						  (float *)&colors[ImGuiCol_PopupBg]);
		ImGui::ColorEdit4("Border", (float *)&colors[ImGuiCol_Border]);
		ImGui::ColorEdit4("Border Shadow",
						  (float *)&colors[ImGuiCol_BorderShadow]);
	}

	// Frame colors
	if (ImGui::CollapsingHeader("Frame Colors",
								ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::ColorEdit4("Frame Background",
						  (float *)&colors[ImGuiCol_FrameBg]);
		ImGui::ColorEdit4("Frame Background Hovered",
						  (float *)&colors[ImGuiCol_FrameBgHovered]);
		ImGui::ColorEdit4("Frame Background Active",
						  (float *)&colors[ImGuiCol_FrameBgActive]);
	}

	// Title bar colors
	if (ImGui::CollapsingHeader("Title Bar Colors")) {
		ImGui::ColorEdit4("Title Background",
						  (float *)&colors[ImGuiCol_TitleBg]);
		ImGui::ColorEdit4("Title Background Active",
						  (float *)&colors[ImGuiCol_TitleBgActive]);
		ImGui::ColorEdit4("Title Background Collapsed",
						  (float *)&colors[ImGuiCol_TitleBgCollapsed]);
	}

	// Button colors
	if (ImGui::CollapsingHeader("Button Colors",
								ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::ColorEdit4("Button", (float *)&colors[ImGuiCol_Button]);
		ImGui::ColorEdit4("Button Hovered",
						  (float *)&colors[ImGuiCol_ButtonHovered]);
		ImGui::ColorEdit4("Button Active",
						  (float *)&colors[ImGuiCol_ButtonActive]);
	}

	// Header colors
	if (ImGui::CollapsingHeader("Header Colors")) {
		ImGui::ColorEdit4("Header", (float *)&colors[ImGuiCol_Header]);
		ImGui::ColorEdit4("Header Hovered",
						  (float *)&colors[ImGuiCol_HeaderHovered]);
		ImGui::ColorEdit4("Header Active",
						  (float *)&colors[ImGuiCol_HeaderActive]);
	}

	// Scrollbar colors
	if (ImGui::CollapsingHeader("Scrollbar Colors")) {
		ImGui::ColorEdit4("Scrollbar Background",
						  (float *)&colors[ImGuiCol_ScrollbarBg]);
		ImGui::ColorEdit4("Scrollbar Grab",
						  (float *)&colors[ImGuiCol_ScrollbarGrab]);
		ImGui::ColorEdit4("Scrollbar Grab Hovered",
						  (float *)&colors[ImGuiCol_ScrollbarGrabHovered]);
		ImGui::ColorEdit4("Scrollbar Grab Active",
						  (float *)&colors[ImGuiCol_ScrollbarGrabActive]);
	}

	// Slider colors
	if (ImGui::CollapsingHeader("Slider Colors")) {
		ImGui::ColorEdit4("Slider Grab", (float *)&colors[ImGuiCol_SliderGrab]);
		ImGui::ColorEdit4("Slider Grab Active",
						  (float *)&colors[ImGuiCol_SliderGrabActive]);
	}

	// Tab colors
	if (ImGui::CollapsingHeader("Tab Colors")) {
		ImGui::ColorEdit4("Tab", (float *)&colors[ImGuiCol_Tab]);
		ImGui::ColorEdit4("Tab Hovered", (float *)&colors[ImGuiCol_TabHovered]);
		ImGui::ColorEdit4("Tab Active", (float *)&colors[ImGuiCol_TabActive]);
		ImGui::ColorEdit4("Tab Unfocused",
						  (float *)&colors[ImGuiCol_TabUnfocused]);
		ImGui::ColorEdit4("Tab Unfocused Active",
						  (float *)&colors[ImGuiCol_TabUnfocusedActive]);
	}

	// Plot colors
	if (ImGui::CollapsingHeader("Plot Colors")) {
		ImGui::ColorEdit4("Plot Lines", (float *)&colors[ImGuiCol_PlotLines]);
		ImGui::ColorEdit4("Plot Lines Hovered",
						  (float *)&colors[ImGuiCol_PlotLinesHovered]);
		ImGui::ColorEdit4("Plot Histogram",
						  (float *)&colors[ImGuiCol_PlotHistogram]);
		ImGui::ColorEdit4("Plot Histogram Hovered",
						  (float *)&colors[ImGuiCol_PlotHistogramHovered]);
	}

	// Other colors
	if (ImGui::CollapsingHeader("Other Colors")) {
		ImGui::ColorEdit4("Check Mark", (float *)&colors[ImGuiCol_CheckMark]);
		ImGui::ColorEdit4("Separator", (float *)&colors[ImGuiCol_Separator]);
		ImGui::ColorEdit4("Separator Hovered",
						  (float *)&colors[ImGuiCol_SeparatorHovered]);
		ImGui::ColorEdit4("Separator Active",
						  (float *)&colors[ImGuiCol_SeparatorActive]);
		ImGui::ColorEdit4("Resize Grip", (float *)&colors[ImGuiCol_ResizeGrip]);
		ImGui::ColorEdit4("Resize Grip Hovered",
						  (float *)&colors[ImGuiCol_ResizeGripHovered]);
		ImGui::ColorEdit4("Resize Grip Active",
						  (float *)&colors[ImGuiCol_ResizeGripActive]);
		ImGui::ColorEdit4("Text Selected Background",
						  (float *)&colors[ImGuiCol_TextSelectedBg]);
		ImGui::ColorEdit4("Drag Drop Target",
						  (float *)&colors[ImGuiCol_DragDropTarget]);
		ImGui::ColorEdit4("Navigation Highlight",
						  (float *)&colors[ImGuiCol_NavHighlight]);
		ImGui::ColorEdit4("Modal Window Dim Background",
						  (float *)&colors[ImGuiCol_ModalWindowDimBg]);
	}

	// Style settings
	ImGui::Spacing();
	ImGui::Text(ICON_FA_COGS " Style Settings");
	ImGui::Separator();

	float alpha = style.Alpha;
	if (ImGui::SliderFloat("Global Alpha", &alpha, 0.0f, 1.0f)) {
		style.Alpha = alpha;
	}

	float windowRounding = style.WindowRounding;
	if (ImGui::SliderFloat("Window Rounding", &windowRounding, 0.0f, 12.0f)) {
		style.WindowRounding = windowRounding;
	}

	float frameRounding = style.FrameRounding;
	if (ImGui::SliderFloat("Frame Rounding", &frameRounding, 0.0f, 12.0f)) {
		style.FrameRounding = frameRounding;
	}

	float grabRounding = style.GrabRounding;
	if (ImGui::SliderFloat("Grab Rounding", &grabRounding, 0.0f, 12.0f)) {
		style.GrabRounding = grabRounding;
	}

	ImVec2 windowPadding = style.WindowPadding;
	if (ImGui::SliderFloat2("Window Padding", (float *)&windowPadding, 0.0f,
							20.0f)) {
		style.WindowPadding = windowPadding;
	}

	ImVec2 framePadding = style.FramePadding;
	if (ImGui::SliderFloat2("Frame Padding", (float *)&framePadding, 0.0f,
							20.0f)) {
		style.FramePadding = framePadding;
	}

	float itemSpacing = style.ItemSpacing.x;
	if (ImGui::SliderFloat("Item Spacing", &itemSpacing, 0.0f, 20.0f)) {
		style.ItemSpacing = ImVec2(itemSpacing, itemSpacing);
	}
}

} // namespace blot
