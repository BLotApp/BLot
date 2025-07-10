#pragma once

#include "TextRenderer.h"
#include "imgui.h"
#include <memory>

class ImGuiRenderer {
public:
    ImGuiRenderer(TextRenderer* textRenderer);
    ~ImGuiRenderer();
    
    void init();
    void render();
    void setCustomFont(const std::string& fontPath, int fontSize = 16);
    
    // Enhanced text rendering methods
    void renderText(const std::string& text, float x, float y, float scale, const ImVec4& color);
    void renderTextCentered(const std::string& text, float x, float y, float scale, const ImVec4& color);
    
    // ImGui integration helpers
    void pushCustomFont();
    void popCustomFont();
    
private:
    TextRenderer* m_textRenderer;
    bool m_useCustomFont;
    std::string m_customFontPath;
    int m_customFontSize;
    
    // ImGui font atlas
    ImFont* m_customImGuiFont;
    ImFontAtlas* m_fontAtlas;
}; 