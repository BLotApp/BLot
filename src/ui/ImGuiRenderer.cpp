#include "ui/ImGuiRenderer.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include "ui/TextRenderer.h"

ImGuiRenderer::ImGuiRenderer(TextRenderer* textRenderer)
    : m_textRenderer(textRenderer)
    , m_useCustomFont(false)
    , m_customFontSize(16)
    , m_customImGuiFont(nullptr)
    , m_fontAtlas(nullptr)
{
}

ImGuiRenderer::~ImGuiRenderer() {
}

void ImGuiRenderer::init() {
    // Initialize with default ImGui font
    ImGuiIO& io = ImGui::GetIO();
    m_fontAtlas = io.Fonts;
    
    // Set up default font
    m_customImGuiFont = m_fontAtlas->AddFontDefault();
    
    // Configure font atlas
    m_fontAtlas->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight;
    m_fontAtlas->Flags |= ImFontAtlasFlags_NoMouseCursors;
}

void ImGuiRenderer::render() {
    // This method can be used for custom rendering outside of ImGui's normal flow
    // For example, rendering text directly to the canvas
}

void ImGuiRenderer::setCustomFont(const std::string& fontPath, int fontSize) {
    m_customFontPath = fontPath;
    m_customFontSize = fontSize;
    
    // Load the font into ImGui's font atlas
    ImGuiIO& io = ImGui::GetIO();
    
    // Clear existing fonts
    io.Fonts->Clear();
    
    // Add the custom font
    m_customImGuiFont = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), fontSize);
    
    if (!m_customImGuiFont) {
        std::cerr << "Failed to load custom font: " << fontPath << std::endl;
        // Fall back to default font
        m_customImGuiFont = io.Fonts->AddFontDefault();
    }
    
    // Also set it in our text renderer
    if (m_textRenderer) {
        m_textRenderer->setFont(fontPath, fontSize);
    }
    
    m_useCustomFont = true;
}

void ImGuiRenderer::renderText(const std::string& text, float x, float y, float scale, const ImVec4& color) {
    if (!m_textRenderer) return;
    
    glm::vec4 glmColor(color.x, color.y, color.z, color.w);
    m_textRenderer->renderText(text, x, y, scale, glmColor);
}

void ImGuiRenderer::renderTextCentered(const std::string& text, float x, float y, float scale, const ImVec4& color) {
    if (!m_textRenderer) return;
    
    // Save current alignment
    auto currentAlignment = m_textRenderer->getTextAlignment();
    
    // Set to center alignment
    m_textRenderer->setTextAlignment(TextAlignment::Center);
    
    // Render text
    glm::vec4 glmColor(color.x, color.y, color.z, color.w);
    m_textRenderer->renderText(text, x, y, scale, glmColor);
    
    // Restore alignment
    m_textRenderer->setTextAlignment(currentAlignment);
}

void ImGuiRenderer::pushCustomFont() {
    if (m_useCustomFont && m_customImGuiFont) {
        ImGui::PushFont(m_customImGuiFont);
    }
}

void ImGuiRenderer::popCustomFont() {
    if (m_useCustomFont) {
        ImGui::PopFont();
    }
} 