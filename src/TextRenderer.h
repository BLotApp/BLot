#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

enum class TextAlignment { Left, Center, Right };

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();
    
    bool init();
    void setFont(const std::string& fontPath, int fontSize = 16);
    void renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color);
    void renderText(const std::string& text, float x, float y, float scale, const glm::vec4& color);
    
    // ImGui integration
    void renderTextForImGui(const std::string& text, float x, float y, float scale, const glm::vec4& color);
    void setImGuiFont(const std::string& fontPath, int fontSize = 16);
    
    // Utility functions
    glm::vec2 getTextSize(const std::string& text, float scale = 1.0f);
    void setTextAlignment(TextAlignment alignment);
    TextAlignment getTextAlignment() const;
    
private:
    void initShaders();
    void initBuffers();
    void loadCharacters();
    
    // FreeType
    FT_Library m_ft;
    FT_Face m_face;
    std::string m_currentFontPath;
    int m_currentFontSize;
    
    // PIMPL for OpenGL resources and character atlas
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    
    // State
    TextAlignment m_alignment;
    glm::vec4 m_currentColor;
    
    // ImGui integration
    bool m_useImGuiRendering;
}; 