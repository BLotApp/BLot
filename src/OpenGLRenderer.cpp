#include <glad/gl.h>
#include "OpenGLRenderer.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"

struct OpenGLRenderer::Impl {
	std::unique_ptr<Shader> basicShader;
	std::unique_ptr<Shader> textShader;
	std::unique_ptr<VertexBuffer> vertexBuffer;
	struct Glyph {
		unsigned int texture;
		glm::vec2 size;
		glm::vec2 bearing;
		float advance;
	};
	std::map<char, Glyph> glyphs;
	unsigned int fontTexture = 0;
};

OpenGLRenderer::OpenGLRenderer() : m_initialized(false), m_width(0), m_height(0), m_impl(std::make_unique<Impl>()) {
	m_impl->vertexBuffer = std::make_unique<VertexBuffer>();
}
OpenGLRenderer::~OpenGLRenderer() {}

bool OpenGLRenderer::initialize(int width, int height) {
    m_width = width;
    m_height = height;
    m_initialized = true;
    std::cout << "[OpenGLRenderer] Initialized with " << width << "x" << height << std::endl;
    return true;
}

void OpenGLRenderer::shutdown() {
    m_initialized = false;
    std::cout << "[OpenGLRenderer] Shutdown" << std::endl;
}

void OpenGLRenderer::resize(int width, int height) {
    m_width = width;
    m_height = height;
    std::cout << "[OpenGLRenderer] Resize to " << width << "x" << height << std::endl;
}

void OpenGLRenderer::beginFrame() {}
void OpenGLRenderer::endFrame() {}
void OpenGLRenderer::clear(const glm::vec4& color) {}

void OpenGLRenderer::drawLine(float x1, float y1, float x2, float y2, float strokeWidth, const glm::vec4& color) {}
void OpenGLRenderer::drawRect(float x, float y, float width, float height, const glm::vec4& fillColor, const glm::vec4& strokeColor, float strokeWidth) {}
void OpenGLRenderer::drawCircle(float x, float y, float radius, const glm::vec4& fillColor, const glm::vec4& strokeColor, float strokeWidth) {}
void OpenGLRenderer::drawEllipse(float x, float y, float width, float height, const glm::vec4& fillColor, const glm::vec4& strokeColor, float strokeWidth) {}
void OpenGLRenderer::drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, const glm::vec4& fillColor, const glm::vec4& strokeColor, float strokeWidth) {}
void OpenGLRenderer::drawPolygon(const std::vector<glm::vec2>& points, const glm::vec4& fillColor, const glm::vec4& strokeColor, float strokeWidth) {}

void OpenGLRenderer::beginPath() {}
void OpenGLRenderer::moveTo(float x, float y) {}
void OpenGLRenderer::lineTo(float x, float y) {}
void OpenGLRenderer::curveTo(float cx1, float cy1, float cx2, float cy2, float x, float y) {}
void OpenGLRenderer::closePath() {}
void OpenGLRenderer::fill(const glm::vec4& color) {}
void OpenGLRenderer::stroke(const glm::vec4& color, float width) {}

void OpenGLRenderer::setFont(const std::string& fontPath, float size) {}
void OpenGLRenderer::drawText(const std::string& text, float x, float y, const glm::vec4& color) {}
glm::vec2 OpenGLRenderer::getTextBounds(const std::string& text) { return glm::vec2(0.0f); }

void OpenGLRenderer::pushMatrix() {}
void OpenGLRenderer::popMatrix() {}
void OpenGLRenderer::translate(float x, float y) {}
void OpenGLRenderer::rotate(float angle) {}
void OpenGLRenderer::scale(float sx, float sy) {}
void OpenGLRenderer::resetMatrix() {}

bool OpenGLRenderer::saveToFile(const std::string& filename) { return false; }
bool OpenGLRenderer::saveToMemory(std::vector<uint8_t>& data) { return false; } 