#include "ThemePanel.h"
#include "ToolbarWindow.h"
#include <imgui.h>
#include <cstdlib>

namespace blot {

ThemePanel::ThemePanel(const std::string& title, Flags flags)
    : Window(title, flags) {
}

void ThemePanel::render() {
    if (m_isOpen) {
        bool open = m_isOpen;
        if (ImGui::Begin(m_title.c_str(), &open, m_flags)) {
            renderRandomThemeButton();
            renderColorPresets();
            renderThemeControls();
        }
        ImGui::End();
        m_isOpen = open;
    }
}

void ThemePanel::renderRandomThemeButton() {
    if (ImGui::Button("🎨 Random Theme")) {
        // Generate random colors for fun!
        auto& style = ImGui::GetStyle();
        auto& colors = style.Colors;
        
        // Random color generator
        auto randomColor = []() {
            return ImVec4(
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX,
                1.0f
            );
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
        
        printf("[ThemePanel] 🌈 Applied random theme! Colors are now completely chaotic!\n");
    }
}

void ThemePanel::renderColorPresets() {
    if (!m_toolbarWindow) return;
    
    ImGui::SameLine();
    
    // Quick color presets
    if (ImGui::Button("🎨 Random Fill")) {
        ImVec4 randomFill = ImVec4(
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX,
            1.0f
        );
        m_toolbarWindow->setFillColor(randomFill);
        printf("[ThemePanel] 🎨 Random fill color: (%.2f, %.2f, %.2f)\n", randomFill.x, randomFill.y, randomFill.z);
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("⚫ No Fill")) {
        m_toolbarWindow->setFillColor(ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        printf("[ThemePanel] ⚫ Disabled fill\n");
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("🔴 Red Fill")) {
        m_toolbarWindow->setFillColor(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        printf("[ThemePanel] 🔴 Set fill to red\n");
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("🟢 Green Fill")) {
        m_toolbarWindow->setFillColor(ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
        printf("[ThemePanel] 🟢 Set fill to green\n");
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("🔵 Blue Fill")) {
        m_toolbarWindow->setFillColor(ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
        printf("[ThemePanel] 🔵 Set fill to blue\n");
    }
}

void ThemePanel::renderThemeControls() {
    ImGui::Separator();
    ImGui::Text("Theme Controls:");
    // TODO: Add more theme controls as needed
}

} // namespace blot 