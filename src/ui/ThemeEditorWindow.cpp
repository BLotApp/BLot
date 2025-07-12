#include "ThemeEditorWindow.h"
#include <imgui.h>
#include <fstream>
#include <nlohmann/json.hpp>

namespace blot {

ThemeEditorWindow::ThemeEditorWindow(const std::string& title, Flags flags)
    : Window(title, flags) {}

void ThemeEditorWindow::setThemePath(std::string* lastThemePath) {
    m_lastThemePath = lastThemePath;
}

void ThemeEditorWindow::setCurrentTheme(int* currentTheme) {
    m_currentTheme = currentTheme;
}

void ThemeEditorWindow::render() {
    begin();
    if (isOpen() && m_lastThemePath && m_currentTheme) {
        renderThemeEditor();
    }
    end();
}

void ThemeEditorWindow::renderThemeEditor() {
    ImGui::Begin("ImGui Theme Editor", nullptr);
    
    // Theme selection
    const char* themes[] = { "Dark", "Light", "Classic", "Corporate", "Dracula" };
    if (ImGui::Combo("Theme", m_currentTheme, themes, IM_ARRAYSIZE(themes))) {
        // Theme change logic would go here
    }
    
    // Save/Load theme buttons
    if (ImGui::Button("Save Current Theme")) {
        // Save theme logic would go here
    }
    ImGui::SameLine();
    if (ImGui::Button("Load Theme")) {
        // Load theme logic would go here
    }
    
    // Theme path display
    ImGui::Text("Last Theme Path: %s", m_lastThemePath->c_str());
    
    // Color customization (simplified)
    ImGui::Separator();
    ImGui::Text("Color Customization");
    
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    
    // A few key colors for demonstration
    ImGui::ColorEdit4("Text", (float*)&colors[ImGuiCol_Text]);
    ImGui::ColorEdit4("Window Background", (float*)&colors[ImGuiCol_WindowBg]);
    ImGui::ColorEdit4("Button", (float*)&colors[ImGuiCol_Button]);
    ImGui::ColorEdit4("Button Hovered", (float*)&colors[ImGuiCol_ButtonHovered]);
    ImGui::ColorEdit4("Button Active", (float*)&colors[ImGuiCol_ButtonActive]);
    
    ImGui::End();
}

} // namespace blot 