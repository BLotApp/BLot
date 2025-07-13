#include "ThemeEditorWindow.h"
#include "../UIManager.h"
#include <imgui.h>
#include <fstream>
#include <nlohmann/json.hpp>

namespace blot {

ThemeEditorWindow::ThemeEditorWindow(const std::string& title, Flags flags)
    : Window(title, flags) {}

void ThemeEditorWindow::setUIManager(UIManager* uiManager) {
    m_uiManager = uiManager;
}

void ThemeEditorWindow::render() {
    if (!isOpen() || !m_uiManager) return;
    
    ImGui::Begin(getTitle().c_str(), &m_isOpen, static_cast<ImGuiWindowFlags>(getFlags()));
    renderThemeEditor();
    ImGui::End();
}

void ThemeEditorWindow::renderThemeEditor() {
    // Theme selection
    const char* themes[] = { "Dark", "Light", "Classic", "Corporate", "Dracula" };
    int currentTheme = static_cast<int>(m_uiManager->m_currentTheme);
    if (ImGui::Combo("Theme", &currentTheme, themes, IM_ARRAYSIZE(themes))) {
        m_uiManager->setImGuiTheme(static_cast<UIManager::ImGuiTheme>(currentTheme));
    }
    
    // Save/Load theme buttons
    if (ImGui::Button("Save Current Theme")) {
        m_uiManager->saveCurrentTheme(m_uiManager->m_lastThemePath);
    }
    ImGui::SameLine();
    if (ImGui::Button("Load Theme")) {
        m_uiManager->loadTheme(m_uiManager->m_lastThemePath);
    }
    
    // Theme path display
    ImGui::Text("Last Theme Path: %s", m_uiManager->m_lastThemePath.c_str());
    
    // Color customization (simplified)
    ImGui::Separator();
    ImGui::Text("Color Customization");
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    ImGui::ColorEdit4("Text", (float*)&colors[ImGuiCol_Text]);
    ImGui::ColorEdit4("Window Background", (float*)&colors[ImGuiCol_WindowBg]);
    ImGui::ColorEdit4("Button", (float*)&colors[ImGuiCol_Button]);
    ImGui::ColorEdit4("Button Hovered", (float*)&colors[ImGuiCol_ButtonHovered]);
    ImGui::ColorEdit4("Button Active", (float*)&colors[ImGuiCol_ButtonActive]);
}

} // namespace blot 