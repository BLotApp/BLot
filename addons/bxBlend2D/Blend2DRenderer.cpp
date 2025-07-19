#ifdef _WIN32
#include <windows.h>
#endif
#include "rendering/gladGlfw.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <algorithm>
#include <blend2d.h>
#include <cstdint>
#include <iostream>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

#include "addons/bxBlend2D/Blend2DRenderer.h"

// Shader sources
const char *vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inUV;
out vec2 fragUV;
void main() {
    fragUV = inUV;
    gl_Position = vec4(inPos, 0.0, 1.0);
}
)";

const char *fragmentShaderSrc = R"(
#version 330 core
in vec2 fragUV;
out vec4 outColor;
uniform sampler2D uTex;
void main() {
    outColor = texture(uTex, fragUV);
}
)";

// Helper for shader compilation
GLuint compileShader(GLenum type, const char *src) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		spdlog::error("Shader compilation failed: {}", infoLog);
	}
	return shader;
}

Blend2DRenderer::Blend2DRenderer()
	: m_initialized(false), m_width(0), m_height(0) {
	// Initialize stroke options with defaults
	m_strokeOptions.width = 1.0;
	m_strokeOptions.startCap = BL_STROKE_CAP_BUTT;
	m_strokeOptions.endCap = BL_STROKE_CAP_BUTT;
	m_strokeOptions.join = BL_STROKE_JOIN_MITER_CLIP;
	m_strokeOptions.miterLimit = 4.0;
	m_strokeOptions.dashOffset = 0.0;
	m_strokeOptions.transformOrder = BL_STROKE_TRANSFORM_ORDER_AFTER;
}

Blend2DRenderer::~Blend2DRenderer() {}

bool Blend2DRenderer::initialize(int width, int height) {
	if (m_context)
		m_context.end();
	m_width = width;
	m_height = height;
	m_pixelBuffer.resize(width * height * 4);
	m_image.createFromData(width, height, BL_FORMAT_PRGB32,
						   m_pixelBuffer.data(), width * 4, BL_DATA_ACCESS_RW,
						   nullptr, nullptr);
	m_context.begin(m_image);
	m_context.setCompOp(BL_COMP_OP_SRC_COPY);
	m_context.fillAll(BLRgba32(0xFFFFFFFF));
	m_context.setCompOp(BL_COMP_OP_SRC_OVER);
	// OpenGL texture setup
	if (m_textureId == 0)
		glGenTextures(1, &m_textureId);
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA,
				 GL_UNSIGNED_BYTE, m_pixelBuffer.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Modern OpenGL: VAO/VBO setup
	if (m_vao == 0) {
		static const float quadVertices[] = {
			-1.0f, -1.0f, 0.0f, 1.0f, 1.0f,	 -1.0f, 1.0f, 1.0f,
			1.0f,  1.0f,  1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 1.0f,
			1.0f,  1.0f,  1.0f, 0.0f, -1.0f, 1.0f,	0.0f, 0.0f};
		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices,
					 GL_STATIC_DRAW);
		// Position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
							  (void *)0);
		// UV attribute
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
							  (void *)(2 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// Shader program
	if (m_shaderProgram == 0) {
		GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSrc);
		GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
		m_shaderProgram = glCreateProgram();
		glAttachShader(m_shaderProgram, vs);
		glAttachShader(m_shaderProgram, fs);
		glLinkProgram(m_shaderProgram);
		glDeleteShader(vs);
		glDeleteShader(fs);
	}
	m_initialized = true;
	return true;
}

void Blend2DRenderer::shutdown() {
	m_context.end();
	m_initialized = false;
	spdlog::info("[Blend2DRenderer] Shutdown");
}

void Blend2DRenderer::resize(int width, int height) {
	initialize(width, height);
}

uint8_t *Blend2DRenderer::getPixelBuffer() { return m_pixelBuffer.data(); }
int Blend2DRenderer::getWidth() const { return m_width; }
int Blend2DRenderer::getHeight() const { return m_height; }

void Blend2DRenderer::beginFrame() {
	spdlog::info("[Blend2DRenderer] Begin Frame");
}
void Blend2DRenderer::endFrame() {
	spdlog::info("[Blend2DRenderer] End Frame");
}

void Blend2DRenderer::clear(const glm::vec4 &color) {
	BLRgba32 c((uint8_t)(color.r * 255), (uint8_t)(color.g * 255),
			   (uint8_t)(color.b * 255), (uint8_t)(color.a * 255));
	m_context.setCompOp(BL_COMP_OP_SRC_COPY);
	m_context.fillAll(c);
}

void Blend2DRenderer::drawLine(float x1, float y1, float x2, float y2) {
	if (!m_hasStroke || m_strokeWidth <= 0.0f)
		return;

	BLRgba32 strokeC(
		uint8_t(m_strokeColor.r * 255), uint8_t(m_strokeColor.g * 255),
		uint8_t(m_strokeColor.b * 255), uint8_t(m_strokeColor.a * 255));

	m_context.setStrokeStyle(strokeC);
	updateStrokeOptions();
	m_context.strokeLine(BLPoint(x1, y1), BLPoint(x2, y2));
}

void Blend2DRenderer::fill(const glm::vec4 &color) {
	m_currentFillColor = color;
}

void Blend2DRenderer::stroke(const glm::vec4 &color, float width) {
	m_currentStrokeColor = color;
	m_currentStrokeWidth = width;
}

void Blend2DRenderer::noFill() { m_hasFill = false; }

void Blend2DRenderer::noStroke() { m_hasStroke = false; }

// Comprehensive stroke options
void Blend2DRenderer::setStrokeCap(BLStrokeCap cap) {
	m_strokeOptions.startCap = static_cast<uint8_t>(cap);
	m_strokeOptions.endCap = static_cast<uint8_t>(cap);
}

void Blend2DRenderer::setStrokeJoin(BLStrokeJoin join) {
	m_strokeOptions.join = static_cast<uint8_t>(join);
}

void Blend2DRenderer::setStrokeMiterLimit(double limit) {
	m_strokeOptions.miterLimit = limit;
}

void Blend2DRenderer::setStrokeDashArray(const std::vector<double> &dashes) {
	m_strokeOptions.dashArray.clear();
	for (double dash : dashes) {
		m_strokeOptions.dashArray.append(dash);
	}
}

void Blend2DRenderer::setStrokeDashOffset(double offset) {
	m_strokeOptions.dashOffset = offset;
}

void Blend2DRenderer::setStrokeTransformOrder(BLStrokeTransformOrder order) {
	m_strokeOptions.transformOrder = static_cast<uint8_t>(order);
}

void Blend2DRenderer::setStrokeOptions(const BLStrokeOptions &options) {
	m_strokeOptions = options;
}

void Blend2DRenderer::setStrokeDashPattern(const std::vector<double> &dashes,
										   double offset) {
	setStrokeDashArray(dashes);
	setStrokeDashOffset(offset);
}

void Blend2DRenderer::updateStrokeOptions() {
	m_strokeOptions.width = m_strokeWidth;
	m_context.setStrokeWidth(m_strokeOptions.width);
	m_context.setStrokeCap(BL_STROKE_CAP_POSITION_START,
						   static_cast<BLStrokeCap>(m_strokeOptions.startCap));
	m_context.setStrokeCap(BL_STROKE_CAP_POSITION_END,
						   static_cast<BLStrokeCap>(m_strokeOptions.endCap));
	m_context.setStrokeJoin(static_cast<BLStrokeJoin>(m_strokeOptions.join));
	m_context.setStrokeMiterLimit(m_strokeOptions.miterLimit);
	m_context.setStrokeDashOffset(m_strokeOptions.dashOffset);
	m_context.setStrokeTransformOrder(
		static_cast<BLStrokeTransformOrder>(m_strokeOptions.transformOrder));

	// Set dash array if not empty
	if (m_strokeOptions.dashArray.size() > 0) {
		m_context.setStrokeDashArray(m_strokeOptions.dashArray);
	} else {
		// Clear dash array
		BLArray<double> emptyArray;
		m_context.setStrokeDashArray(emptyArray);
	}
}

void Blend2DRenderer::setFillColor(const glm::vec4 &color) {
	m_fillColor = color;
}

void Blend2DRenderer::setStrokeColor(const glm::vec4 &color) {
	m_strokeColor = color;
}

void Blend2DRenderer::setStrokeWidth(float width) { m_strokeWidth = width; }

void Blend2DRenderer::drawRect(float x, float y, float width, float height) {
	BLRgba32 fill(uint8_t(m_fillColor.r * 255), uint8_t(m_fillColor.g * 255),
				  uint8_t(m_fillColor.b * 255), uint8_t(m_fillColor.a * 255));
	BLRgba32 stroke(
		uint8_t(m_strokeColor.r * 255), uint8_t(m_strokeColor.g * 255),
		uint8_t(m_strokeColor.b * 255), uint8_t(m_strokeColor.a * 255));
	m_context.setFillStyle(fill);
	m_context.setStrokeStyle(stroke);
	updateStrokeOptions();
	m_context.fillRect(BLRect(x, y, width, height));
	if (m_strokeWidth > 0.0f) {
		m_context.strokeRect(BLRect(x, y, width, height));
	}
}

void Blend2DRenderer::drawCircle(float x, float y, float radius) {
	BLEllipse ellipse(x, y, radius, radius);
	if (m_hasFill) {
		BLRgba32 fillC(
			uint8_t(m_fillColor.r * 255), uint8_t(m_fillColor.g * 255),
			uint8_t(m_fillColor.b * 255), uint8_t(m_fillColor.a * 255));
		m_context.setCompOp(BL_COMP_OP_SRC_OVER);
		m_context.setFillStyle(fillC);
		m_context.fillEllipse(ellipse);
	}
	if (m_hasStroke && m_strokeWidth > 0.0f) {
		BLRgba32 strokeC(
			uint8_t(m_strokeColor.r * 255), uint8_t(m_strokeColor.g * 255),
			uint8_t(m_strokeColor.b * 255), uint8_t(m_strokeColor.a * 255));
		m_context.setStrokeStyle(strokeC);
		updateStrokeOptions();
		m_context.strokeEllipse(ellipse);
	}
}

void Blend2DRenderer::drawEllipse(float x, float y, float width, float height) {
	BLEllipse ellipse(x, y, width * 0.5, height * 0.5);
	if (m_hasFill) {
		BLRgba32 fillC(
			uint8_t(m_fillColor.r * 255), uint8_t(m_fillColor.g * 255),
			uint8_t(m_fillColor.b * 255), uint8_t(m_fillColor.a * 255));
		m_context.setCompOp(BL_COMP_OP_SRC_OVER);
		m_context.setFillStyle(fillC);
		m_context.fillEllipse(ellipse);
	}
	if (m_hasStroke && m_strokeWidth > 0.0f) {
		BLRgba32 strokeC(
			uint8_t(m_strokeColor.r * 255), uint8_t(m_strokeColor.g * 255),
			uint8_t(m_strokeColor.b * 255), uint8_t(m_strokeColor.a * 255));
		m_context.setStrokeStyle(strokeC);
		updateStrokeOptions();
		m_context.strokeEllipse(ellipse);
	}
}

void Blend2DRenderer::drawTriangle(float x1, float y1, float x2, float y2,
								   float x3, float y3) {
	BLPath path;
	path.moveTo(x1, y1);
	path.lineTo(x2, y2);
	path.lineTo(x3, y3);
	path.close();
	if (m_hasFill) {
		BLRgba32 fillC(
			uint8_t(m_fillColor.r * 255), uint8_t(m_fillColor.g * 255),
			uint8_t(m_fillColor.b * 255), uint8_t(m_fillColor.a * 255));
		m_context.setCompOp(BL_COMP_OP_SRC_OVER);
		m_context.setFillStyle(fillC);
		m_context.fillPath(path);
	}
	if (m_hasStroke && m_strokeWidth > 0.0f) {
		BLRgba32 strokeC(
			uint8_t(m_strokeColor.r * 255), uint8_t(m_strokeColor.g * 255),
			uint8_t(m_strokeColor.b * 255), uint8_t(m_strokeColor.a * 255));
		m_context.setStrokeStyle(strokeC);
		updateStrokeOptions();
		m_context.strokePath(path);
	}
}

void Blend2DRenderer::drawPolygon(const std::vector<glm::vec2> &points) {
	if (points.size() < 3)
		return;
	BLPath path;
	path.moveTo(points[0].x, points[0].y);
	for (size_t i = 1; i < points.size(); ++i) {
		path.lineTo(points[i].x, points[i].y);
	}
	path.close();
	if (m_hasFill) {
		BLRgba32 fillC(
			uint8_t(m_fillColor.r * 255), uint8_t(m_fillColor.g * 255),
			uint8_t(m_fillColor.b * 255), uint8_t(m_fillColor.a * 255));
		m_context.setCompOp(BL_COMP_OP_SRC_OVER);
		m_context.setFillStyle(fillC);
		m_context.fillPath(path);
	}
	if (m_hasStroke && m_strokeWidth > 0.0f) {
		BLRgba32 strokeC(
			uint8_t(m_strokeColor.r * 255), uint8_t(m_strokeColor.g * 255),
			uint8_t(m_strokeColor.b * 255), uint8_t(m_strokeColor.a * 255));
		m_context.setStrokeStyle(strokeC);
		updateStrokeOptions();
		m_context.strokePath(path);
	}
}

void Blend2DRenderer::beginPath() {}
void Blend2DRenderer::moveTo(float x, float y) {}
void Blend2DRenderer::lineTo(float x, float y) {}
void Blend2DRenderer::curveTo(float cx1, float cy1, float cx2, float cy2,
							  float x, float y) {}
void Blend2DRenderer::closePath() {}

void Blend2DRenderer::setFont(const std::string &fontPath, float size) {}
void Blend2DRenderer::drawText(const std::string &text, float x, float y,
							   const glm::vec4 &color) {}
glm::vec2 Blend2DRenderer::getTextBounds(const std::string &text) {
	return glm::vec2(0.0f);
}

void Blend2DRenderer::pushMatrix() {}
void Blend2DRenderer::popMatrix() {}
void Blend2DRenderer::translate(float x, float y) {}
void Blend2DRenderer::rotate(float angle) {}
void Blend2DRenderer::scale(float sx, float sy) {}
void Blend2DRenderer::resetMatrix() {}

bool Blend2DRenderer::saveToFile(const std::string &filename) { return false; }
bool Blend2DRenderer::saveToMemory(std::vector<uint8_t> &data) { return false; }

void Blend2DRenderer::flush() {
	m_context.end();
	m_context.begin(m_image);
}

void Blend2DRenderer::present() {
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_BGRA,
					GL_UNSIGNED_BYTE, m_pixelBuffer.data());
	glUseProgram(m_shaderProgram);
	glBindVertexArray(m_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glUniform1i(glGetUniformLocation(m_shaderProgram, "uTex"), 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glUseProgram(0);
}
