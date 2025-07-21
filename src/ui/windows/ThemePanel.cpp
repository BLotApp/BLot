#include "ThemePanel.h"
#include <cstdlib>
#include <imgui.h>
#include "../third_party/IconFontCppHeaders/IconsFontAwesome5.h"

namespace blot {

ThemePanel::ThemePanel(const std::string &title, Flags flags)
	: Window(title, flags) {}

void ThemePanel::renderContents() {
	renderRandomThemeButton();
	renderColorPresets();
	renderThemeControls();
}

void ThemePanel::renderRandomThemeButton() {
	if (ImGui::Button(ICON_FA_PALETTE " Random Theme")) {
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
		colors[ImGuiCol_TableHeaderBg] = randomColor();
		colors[ImGuiCol_TableBorderLight] = randomColor();
		colors[ImGuiCol_TableBorderStrong] = randomColor();
		colors[ImGuiCol_TableRowBg] = randomColor();
		colors[ImGuiCol_TableRowBgAlt] = randomColor();
		colors[ImGuiCol_TextSelectedBg] = randomColor();
		colors[ImGuiCol_DragDropTarget] = randomColor();
		colors[ImGuiCol_NavHighlight] = randomColor();
		colors[ImGuiCol_NavWindowingHighlight] = randomColor();
		colors[ImGuiCol_NavWindowingDimBg] = randomColor();
		colors[ImGuiCol_ModalWindowDimBg] = randomColor();

		printf("[ThemePanel] Applied random theme! Colors are now completely "
			   "chaotic!\n");
	}
}

void ThemePanel::renderColorPresets() {

	ImGui::SameLine();

	// Quick color presets
	if (ImGui::Button(ICON_FA_PALETTE " Random Fill")) {
		ImVec4 randomFill = ImVec4(static_cast<float>(rand()) / RAND_MAX,
								   static_cast<float>(rand()) / RAND_MAX,
								   static_cast<float>(rand()) / RAND_MAX, 1.0f);
		printf("[ThemePanel] Random fill color: (%.2f, %.2f, %.2f)\n",
			   randomFill.x, randomFill.y, randomFill.z);
	}
}

void ThemePanel::renderThemeControls() {
	ImGui::Separator();
	ImGui::Text("Theme Controls:");

	// Quick theme presets
	if (ImGui::Button(ICON_FA_MOON " Dark Theme")) {
		ImGui::StyleColorsDark();
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_SUN " Light Theme")) {
		ImGui::StyleColorsLight();
	}
	ImGui::SameLine();
	if (ImGui::Button(ICON_FA_PAINT_BRUSH " Classic Theme")) {
		ImGui::StyleColorsClassic();
	}

	ImGui::Separator();
	ImGui::Text("Color Customization:");

	// Direct color editing
	ImGuiStyle &style = ImGui::GetStyle();
	ImVec4 *colors = style.Colors;

	if (ImGui::CollapsingHeader("Text Colors")) {
		ImGui::ColorEdit4("Text", (float *)&colors[ImGuiCol_Text]);
		ImGui::ColorEdit4("Text Disabled",
						  (float *)&colors[ImGuiCol_TextDisabled]);
	}

	if (ImGui::CollapsingHeader("Background Colors")) {
		ImGui::ColorEdit4("Window Background",
						  (float *)&colors[ImGuiCol_WindowBg]);
		ImGui::ColorEdit4("Child Background",
						  (float *)&colors[ImGuiCol_ChildBg]);
		ImGui::ColorEdit4("Popup Background",
						  (float *)&colors[ImGuiCol_PopupBg]);
	}

	if (ImGui::CollapsingHeader("Button Colors")) {
		ImGui::ColorEdit4("Button", (float *)&colors[ImGuiCol_Button]);
		ImGui::ColorEdit4("Button Hovered",
						  (float *)&colors[ImGuiCol_ButtonHovered]);
		ImGui::ColorEdit4("Button Active",
						  (float *)&colors[ImGuiCol_ButtonActive]);
	}

	if (ImGui::CollapsingHeader("Frame Colors")) {
		ImGui::ColorEdit4("Frame Background",
						  (float *)&colors[ImGuiCol_FrameBg]);
		ImGui::ColorEdit4("Frame Hovered",
						  (float *)&colors[ImGuiCol_FrameBgHovered]);
		ImGui::ColorEdit4("Frame Active",
						  (float *)&colors[ImGuiCol_FrameBgActive]);
	}
}

} // namespace blot
