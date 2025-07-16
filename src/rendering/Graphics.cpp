#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "rendering/Graphics.h"
#include "rendering/Blend2DRenderer.h"
#include <blend2d.h>

struct Graphics::Impl {
	GLuint shaderProgram = 0;
	GLuint VAO = 0;
	GLuint VBO = 0;
};

Graphics::Graphics()
	: m_impl(std::make_unique<Impl>())
	, m_fillColor(1.0f, 1.0f, 1.0f, 1.0f)
	, m_strokeColor(0.0f, 0.0f, 0.0f, 1.0f)
	, m_strokeWidth(1.0f)
	, m_fillOpacity(1.0f)
	, m_pathOpen(false)
	, m_fontSize(12.0f)
	, m_textAlign(0)
	, m_blendMode(0)
	, m_hasShadow(false)
	, m_hasGradient(false)
{
	m_currentMatrix = glm::mat4(1.0f);
	initShaders();
}

Graphics::~Graphics() {
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

void Graphics::setFillColor(float r, float g, float b, float a) {
    m_fillColor = glm::vec4(r, g, b, a);
    if (m_renderer) m_renderer->setFillColor(m_fillColor);
}

void Graphics::setStrokeColor(float r, float g, float b, float a) {
    m_strokeColor = glm::vec4(r, g, b, a);
    if (m_renderer) m_renderer->setStrokeColor(m_strokeColor);
}

void Graphics::setStrokeWidth(float width) {
    m_strokeWidth = width;
    if (m_renderer) m_renderer->setStrokeWidth(m_strokeWidth);
}

void Graphics::setFillOpacity(float opacity) {
    m_fillOpacity = opacity;
}

void Graphics::drawRect(float x, float y, float width, float height) {
    if (m_renderer) {
        m_renderer->drawRect(x, y, width, height);
    }
}

void Graphics::drawEllipse(float x, float y, float width, float height) {
    if (m_renderer) {
        m_renderer->drawEllipse(x, y, width, height);
    }
}

void Graphics::drawCircle(float x, float y, float radius) {
    if (m_renderer) {
        m_renderer->drawCircle(x, y, radius);
    }
}

void Graphics::drawLine(float x1, float y1, float x2, float y2) {
    if (m_renderer) {
        m_renderer->drawLine(x1, y1, x2, y2);
    }
}

void Graphics::drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3) {
    if (m_renderer) {
        m_renderer->drawTriangle(x1, y1, x2, y2, x3, y3);
    }
}

void Graphics::drawPolygon(const std::vector<glm::vec2>& points) {
    if (m_renderer) {
        m_renderer->drawPolygon(points);
    }
}

void Graphics::beginPath() {
    m_currentPath.clear();
    m_pathOpen = true;
}

void Graphics::moveTo(float x, float y) {
    if (m_pathOpen) {
        m_currentPath.push_back(glm::vec2(x, y));
    }
}

void Graphics::lineTo(float x, float y) {
    if (m_pathOpen) {
        m_currentPath.push_back(glm::vec2(x, y));
    }
}

void Graphics::curveTo(float x1, float y1, float x2, float y2, float x3, float y3) {
    // Basic curve implementation
    if (m_pathOpen) {
        // Add control points and end point
        m_currentPath.push_back(glm::vec2(x1, y1));
        m_currentPath.push_back(glm::vec2(x2, y2));
        m_currentPath.push_back(glm::vec2(x3, y3));
    }
}

void Graphics::closePath() {
    if (m_pathOpen && !m_currentPath.empty()) {
        m_currentPath.push_back(m_currentPath[0]); // Close the path
        m_pathOpen = false;
    }
}

void Graphics::fill() {
    if (!m_currentPath.empty()) {
        drawPolygon(m_currentPath);
    }
}

void Graphics::stroke() {
    if (m_currentPath.size() > 1) {
        for (size_t i = 0; i < m_currentPath.size() - 1; i++) {
            drawLine(m_currentPath[i].x, m_currentPath[i].y, 
                    m_currentPath[i + 1].x, m_currentPath[i + 1].y);
        }
    }
}

void Graphics::drawText(const std::string& text, float x, float y) {
    // Basic text rendering - in a real implementation, this would use the TextRenderer
    // For now, we'll just draw a placeholder rectangle
    drawRect(x, y, text.length() * m_fontSize * 0.6f, m_fontSize);
}

void Graphics::setFont(const std::string& fontName, float size) {
    m_currentFont = fontName;
    m_fontSize = size;
}

void Graphics::setTextAlign(int align) {
    m_textAlign = align;
}

void Graphics::pushMatrix() {
    m_matrixStack.push_back(m_currentMatrix);
}

void Graphics::popMatrix() {
    if (!m_matrixStack.empty()) {
        m_currentMatrix = m_matrixStack.back();
        m_matrixStack.pop_back();
    }
}

void Graphics::translate(float x, float y) {
    m_currentMatrix = glm::translate(m_currentMatrix, glm::vec3(x, y, 0.0f));
}

void Graphics::rotate(float angle) {
    m_currentMatrix = glm::rotate(m_currentMatrix, angle, glm::vec3(0.0f, 0.0f, 1.0f));
}

void Graphics::scale(float x, float y) {
    m_currentMatrix = glm::scale(m_currentMatrix, glm::vec3(x, y, 1.0f));
}

void Graphics::transform(float a, float b, float c, float d, float e, float f) {
    glm::mat4 transform = glm::mat4(
        a, b, 0.0f, 0.0f,
        c, d, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        e, f, 0.0f, 1.0f
    );
    m_currentMatrix = m_currentMatrix * transform;
}

void Graphics::setBlendMode(int mode) {
    m_blendMode = mode;
}

void Graphics::setShadow(float x, float y, float blur, float r, float g, float b, float a) {
    m_hasShadow = true;
    m_shadowColor = glm::vec4(r, g, b, a);
    m_shadowOffset = glm::vec2(x, y);
    m_shadowBlur = blur;
}

void Graphics::setGradient(float x1, float y1, float r1, float g1, float b1, 
                          float x2, float y2, float r2, float g2, float b2) {
    m_hasGradient = true;
    m_gradientStart = glm::vec2(x1, y1);
    m_gradientEnd = glm::vec2(x2, y2);
    m_gradientStartColor = glm::vec4(r1, g1, b1, 1.0f);
    m_gradientEndColor = glm::vec4(r2, g2, b2, 1.0f);
}

void Graphics::drawImage(const std::string& imagePath, float x, float y, float width, float height) {
    // Basic image drawing - would load and render texture
    (void)imagePath;
    drawRect(x, y, width, height);
}

void Graphics::setImageMode(int mode) {
    // Image mode setting
    (void)mode;
}

void Graphics::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Graphics::save() {
    pushMatrix();
}

void Graphics::restore() {
    popMatrix();
}

void Graphics::initShaders() {
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        
        uniform mat4 model;
        uniform mat4 projection;
        
        void main() {
            gl_Position = projection * model * vec4(aPos, 1.0);
        }
    )";
    
    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        
        uniform vec4 fillColor;
        uniform vec4 strokeColor;
        uniform float strokeWidth;
        
        void main() {
            FragColor = fillColor;
        }
    )";
    
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
    
    glGenVertexArrays(1, &m_impl->VAO);
    glGenBuffers(1, &m_impl->VBO);
    
    glBindVertexArray(m_impl->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_impl->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 1024, NULL, GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    glBindVertexArray(0);
}

void Graphics::updateTransform() {
    // Update transformation matrix
} 

void Graphics::setRenderer(IRenderer* renderer) {
    m_renderer = renderer;
}

void Graphics::setStrokeCap(int cap) {
    if (m_renderer) {
        // Only Blend2DRenderer supports BLStrokeCap, so cast if needed
        if (m_renderer->getType() == RendererType::Blend2D) {
            auto* b2d = static_cast<Blend2DRenderer*>(m_renderer);
            b2d->setStrokeCap(static_cast<BLStrokeCap>(cap));
        }
    }
}
void Graphics::setStrokeJoin(int join) {
    if (m_renderer) {
        if (m_renderer->getType() == RendererType::Blend2D) {
            auto* b2d = static_cast<Blend2DRenderer*>(m_renderer);
            b2d->setStrokeJoin(static_cast<BLStrokeJoin>(join));
        }
    }
}
void Graphics::setStrokeDash(const std::vector<float>& dashes, float offset) {
    if (m_renderer) {
        if (m_renderer->getType() == RendererType::Blend2D) {
            auto* b2d = static_cast<Blend2DRenderer*>(m_renderer);
            std::vector<double> d(dashes.begin(), dashes.end());
            b2d->setStrokeDashPattern(d, static_cast<double>(offset));
        }
    }
} 

void Graphics::setCanvasSize(int width, int height) {
    m_canvasWidth = width;
    m_canvasHeight = height;
} 

void Graphics::rect(float x, float y, float width, float height) {
    if (m_renderer) {
        m_renderer->drawRect(x, y, width, height);
    }
} 