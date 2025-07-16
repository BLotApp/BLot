#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include "ui/TextRenderer.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec4 vertex;
out vec2 TexCoords;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 color;
in vec2 TexCoords;

uniform sampler2D text;
uniform vec3 textColor;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
}
)";

// Move struct Character to file scope above TextRenderer methods if needed
struct Character {
	GLuint textureID;
	glm::ivec2 size;
	glm::ivec2 bearing;
	uint32_t advance;
};

struct TextRenderer::Impl {
	GLuint shaderProgram = 0;
	GLuint VAO = 0;
	GLuint VBO = 0;
	GLuint imGuiTextureID = 0;
	std::unordered_map<char, Character> characters;
};

TextRenderer::TextRenderer()
	: m_ft(nullptr)
	, m_face(nullptr)
	, m_currentFontSize(16)
	, m_impl(std::make_unique<Impl>())
	, m_alignment(TextAlignment::Left)
	, m_currentColor(1.0f, 1.0f, 1.0f, 1.0f)
	, m_useImGuiRendering(false)
{
}

TextRenderer::~TextRenderer() {
	if (m_face) {
		FT_Done_Face(m_face);
	}
	if (m_ft) {
		FT_Done_FreeType(m_ft);
	}
	if (m_impl->shaderProgram) {
		glDeleteProgram(m_impl->shaderProgram);
	}
	if (m_impl->VAO) {
		glDeleteVertexArrays(1, &m_impl->VAO);
	}
	if (m_impl->VBO) {
		glDeleteBuffers(1, &m_impl->VBO);
	}
}

bool TextRenderer::init() {
    // Initialize FreeType
    if (FT_Init_FreeType(&m_ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return false;
    }
    
    initShaders();
    initBuffers();
    
    return true;
}

void TextRenderer::initShaders() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    m_impl->shaderProgram = glCreateProgram();
    glAttachShader(m_impl->shaderProgram, vertexShader);
    glAttachShader(m_impl->shaderProgram, fragmentShader);
    glLinkProgram(m_impl->shaderProgram);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void TextRenderer::initBuffers() {
    glGenVertexArrays(1, &m_impl->VAO);
    glGenBuffers(1, &m_impl->VBO);
    glBindVertexArray(m_impl->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_impl->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::setFont(const std::string& fontPath, int fontSize) {
    if (m_face) {
        FT_Done_Face(m_face);
    }
    
    if (FT_New_Face(m_ft, fontPath.c_str(), 0, &m_face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }
    
    FT_Set_Pixel_Sizes(m_face, 0, fontSize);
    m_currentFontPath = fontPath;
    m_currentFontSize = fontSize;
    
    loadCharacters();
}

void TextRenderer::loadCharacters() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(m_face, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
        }
        
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            m_face->glyph->bitmap.width,
            m_face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            m_face->glyph->bitmap.buffer
        );
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        Character character = {
            texture,
            glm::ivec2(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows),
            glm::ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top),
            static_cast<GLuint>(m_face->glyph->advance.x)
        };
        m_impl->characters.insert(std::pair<char, Character>(c, character));
    }
}

void TextRenderer::renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color) {
    renderText(text, x, y, scale, glm::vec4(color, 1.0f));
}

void TextRenderer::renderText(const std::string& text, float x, float y, float scale, const glm::vec4& color) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUseProgram(m_impl->shaderProgram);
    glUniform3f(glGetUniformLocation(m_impl->shaderProgram, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_impl->VAO);
    
    float startX = x;
    if (m_alignment == TextAlignment::Center) {
        glm::vec2 textSize = getTextSize(text, scale);
        startX -= textSize.x * 0.5f;
    } else if (m_alignment == TextAlignment::Right) {
        glm::vec2 textSize = getTextSize(text, scale);
        startX -= textSize.x;
    }
    
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = m_impl->characters[*c];
        
        float xpos = startX + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;
        
        float w = ch.size.x * scale;
        float h = ch.size.y * scale;
        
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        glBindBuffer(GL_ARRAY_BUFFER, m_impl->VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        startX += (ch.advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::renderTextForImGui(const std::string& text, float x, float y, float scale, const glm::vec4& color) {
    // This method provides enhanced text rendering for ImGui
    // It uses our FreeType-based renderer instead of ImGui's default text rendering
    m_useImGuiRendering = true;
    renderText(text, x, y, scale, color);
    m_useImGuiRendering = false;
}

void TextRenderer::setImGuiFont(const std::string& fontPath, int fontSize) {
    setFont(fontPath, fontSize);
    // Additional ImGui-specific font setup can be added here
}

glm::vec2 TextRenderer::getTextSize(const std::string& text, float scale) {
    float width = 0.0f;
    float height = 0.0f;
    
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = m_impl->characters[*c];
        width += (ch.advance >> 6) * scale;
        height = std::max(height, static_cast<float>(ch.size.y) * scale);
    }
    
    return glm::vec2(width, height);
}

void TextRenderer::setTextAlignment(TextAlignment alignment) { m_alignment = alignment; }
TextAlignment TextRenderer::getTextAlignment() const { return m_alignment; } 