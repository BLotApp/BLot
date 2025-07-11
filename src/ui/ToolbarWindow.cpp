#include "ToolbarWindow.h"
#include "../third_party/IconFontCppHeaders/IconsFontAwesome5.h"
#include <imgui.h>

namespace blot {

ToolbarWindow::ToolbarWindow(const std::string& title, Flags flags)
    : Window(title, flags) {
    // Set initial window size for toolbar
    m_state.size = ImVec2(800, 60);
    m_state.position = ImVec2(10, 10);
}

void ToolbarWindow::setCurrentTool(int toolType) {
    m_currentTool = toolType;
}

void ToolbarWindow::setFillColor(const ImVec4& color) {
    m_fillColor = color;
}

void ToolbarWindow::setStrokeColor(const ImVec4& color) {
    m_strokeColor = color;
}

void ToolbarWindow::setStrokeWidth(float width) {
    m_strokeWidth = width;
}

void ToolbarWindow::setShowSwatches(bool show) {
    m_showSwatches = show;
}

void ToolbarWindow::setShowStrokePalette(bool show) {
    m_showStrokePalette = show;
}

void ToolbarWindow::setOnToolChanged(std::function<void(int)> callback) {
    m_onToolChanged = callback;
}

void ToolbarWindow::setOnFillColorChanged(std::function<void(const ImVec4&)> callback) {
    m_onFillColorChanged = callback;
}

void ToolbarWindow::setOnStrokeColorChanged(std::function<void(const ImVec4&)> callback) {
    m_onStrokeColorChanged = callback;
}

void ToolbarWindow::setOnStrokeWidthChanged(std::function<void(float)> callback) {
    m_onStrokeWidthChanged = callback;
}

void ToolbarWindow::render() {
    // Set window size and position
    ImGui::SetNextWindowSize(m_state.size, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(m_state.position, ImGuiCond_FirstUseEver);
    
    // Begin the window
    if (ImGui::Begin(m_title.c_str(), &m_state.isOpen, m_state.flags)) {
        renderTools();
        ImGui::SameLine();
        renderColors();
        
        if (m_showSwatches) {
            renderSwatches();
        }
        
        if (m_showStrokePalette) {
            renderStrokePalette();
        }
    }
    ImGui::End();
}

void ToolbarWindow::renderTools() {
    ImGui::Text("Tools:");
    ImGui::SameLine();
    
    toolButton(ICON_FA_SQUARE, 0); // Rectangle
    toolButton(ICON_FA_CIRCLE, 1); // Circle
    toolButton(ICON_FA_MINUS, 2);  // Line
    toolButton(ICON_FA_PEN, 3);    // Pen
}

void ToolbarWindow::renderColors() {
    ImGui::SameLine();
    ImGui::Text("Fill:");
    ImGui::SameLine();
    
    if (ImGui::ColorEdit4("##FillColor", (float*)&m_fillColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
        if (m_onFillColorChanged) {
            m_onFillColorChanged(m_fillColor);
        }
    }
    
    ImGui::SameLine();
    ImGui::Text("Stroke:");
    ImGui::SameLine();
    
    if (ImGui::ColorEdit4("##StrokeColor", (float*)&m_strokeColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
        if (m_onStrokeColorChanged) {
            m_onStrokeColorChanged(m_strokeColor);
        }
    }
    
    ImGui::SameLine();
    ImGui::Text("Width:");
    ImGui::SameLine();
    
    if (ImGui::DragFloat("##StrokeWidth", &m_strokeWidth, 0.1f, 0.0f, 50.0f, "%.1f")) {
        if (m_onStrokeWidthChanged) {
            m_onStrokeWidthChanged(m_strokeWidth);
        }
    }
}

void ToolbarWindow::renderSwatches() {
    // Simple color swatches
    ImGui::Separator();
    ImGui::Text("Swatches:");
    
    ImVec4 swatches[] = {
        ImVec4(0,0,0,1), ImVec4(1,1,1,1), ImVec4(1,0,0,1), 
        ImVec4(0,1,0,1), ImVec4(0,0,1,1), ImVec4(1,1,0,1)
    };
    
    for (int i = 0; i < 6; i++) {
        ImGui::SameLine();
        if (ImGui::ColorButton(("##Swatch" + std::to_string(i)).c_str(), swatches[i], ImGuiColorEditFlags_NoInputs, ImVec2(20, 20))) {
            m_fillColor = swatches[i];
            if (m_onFillColorChanged) {
                m_onFillColorChanged(m_fillColor);
            }
        }
    }
}

void ToolbarWindow::renderStrokePalette() {
    ImGui::Separator();
    ImGui::Text("Stroke Options:");
    
    ImGui::DragFloat("Width", &m_strokeWidth, 0.1f, 0.0f, 50.0f, "%.1f");
    
    const char* capNames[] = { "Butt", "Square", "Round" };
    static int capStyle = 0;
    ImGui::Combo("Cap Style", &capStyle, capNames, 3);
    
    const char* joinNames[] = { "Miter", "Bevel", "Round" };
    static int joinStyle = 0;
    ImGui::Combo("Join Style", &joinStyle, joinNames, 3);
}

void ToolbarWindow::toolButton(const char* icon, int toolType) {
    ImGui::PushID(icon);
    bool selected = (m_currentTool == toolType);
    if (selected) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    }
    
    if (ImGui::Button(icon)) {
        m_currentTool = toolType;
        if (m_onToolChanged) {
            m_onToolChanged(toolType);
        }
    }
    
    if (selected) {
        ImGui::PopStyleColor();
    }
    ImGui::PopID();
    ImGui::SameLine();
}

} // namespace blot 