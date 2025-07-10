#include "Blend2DRenderer.h"
#include <iostream>
#include <blend2d.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

Blend2DRenderer::Blend2DRenderer() : m_initialized(false), m_width(0), m_height(0) {}
Blend2DRenderer::~Blend2DRenderer() {}

bool Blend2DRenderer::initialize(int width, int height) {
    m_width = width;
    m_height = height;
    m_image.create(m_width, m_height, BL_FORMAT_PRGB32);
    m_context.begin(m_image);
    m_initialized = true;
    std::cout << "[Blend2DRenderer] Initialized with " << width << "x" << height << std::endl;
    return true;
}

void Blend2DRenderer::shutdown() {
    m_context.end();
    m_initialized = false;
    std::cout << "[Blend2DRenderer] Shutdown" << std::endl;
}

void Blend2DRenderer::resize(int width, int height) {
    m_width = width;
    m_height = height;
    m_image.create(m_width, m_height, BL_FORMAT_PRGB32);
    std::cout << "[Blend2DRenderer] Resize to " << width << "x" << height << std::endl;
}

void Blend2DRenderer::beginFrame() {
    std::cout << "[Blend2DRenderer] Begin Frame" << std::endl;
}
void Blend2DRenderer::endFrame() {
    std::cout << "[Blend2DRenderer] End Frame" << std::endl;
}

void Blend2DRenderer::clear(const glm::vec4& color) {
    BLRgba32 c((uint8_t)(color.r * 255), (uint8_t)(color.g * 255), (uint8_t)(color.b * 255), (uint8_t)(color.a * 255));
    m_context.setCompOp(BL_COMP_OP_SRC_COPY);
    m_context.fillAll(c);
}

void Blend2DRenderer::drawLine(float x1, float y1, float x2, float y2, float strokeWidth, const glm::vec4& color) {}

void Blend2DRenderer::fill(const glm::vec4& color) {
    m_currentFillColor = color;
}

void Blend2DRenderer::stroke(const glm::vec4& color, float width) {
    m_currentStrokeColor = color;
    m_currentStrokeWidth = width;
    m_context.setStrokeCap(BL_STROKE_CAP_POSITION_START, BL_STROKE_CAP_ROUND);
    m_context.setStrokeCap(BL_STROKE_CAP_POSITION_END, BL_STROKE_CAP_ROUND);
    m_context.setStrokeJoin(BL_STROKE_JOIN_ROUND);
}

void Blend2DRenderer::noFill() {
    m_hasFill = false;
}

void Blend2DRenderer::noStroke() {
    m_hasStroke = false;
}

void Blend2DRenderer::setStrokeCap(BLStrokeCap cap) {
    m_strokeCap = cap;
    m_context.setStrokeCap(BL_STROKE_CAP_POSITION_START, cap);
    m_context.setStrokeCap(BL_STROKE_CAP_POSITION_END, cap);
}

void Blend2DRenderer::setStrokeJoin(BLStrokeJoin join) {
    m_strokeJoin = join;
    m_context.setStrokeJoin(join);
}

void Blend2DRenderer::setStrokeDashPattern(const std::vector<double>& dashes, double offset) {
    m_dashPattern = dashes;
    m_dashOffset = offset;
    BLArray<double> dashArray;
    dashArray.clear();
    for (double d : dashes) dashArray.append(d);
    m_context.setStrokeDashArray(dashArray);
    m_context.setStrokeDashOffset(offset);
}

void Blend2DRenderer::drawRect(float x, float y, float width, float height, const glm::vec4& fillColor, const glm::vec4& strokeColor, float strokeWidth) {
    BLRect rect(x, y, width, height);
    if (m_hasFill) {
        BLRgba32 fillC((uint8_t)(m_currentFillColor.r * 255), (uint8_t)(m_currentFillColor.g * 255), (uint8_t)(m_currentFillColor.b * 255), (uint8_t)(m_currentFillColor.a * 255));
        m_context.setCompOp(BL_COMP_OP_SRC_OVER);
        m_context.fillRect(rect, fillC);
    }
    if (m_hasStroke && m_currentStrokeWidth > 0.0f) {
        BLRgba32 strokeC((uint8_t)(m_currentStrokeColor.r * 255), (uint8_t)(m_currentStrokeColor.g * 255), (uint8_t)(m_currentStrokeColor.b * 255), (uint8_t)(m_currentStrokeColor.a * 255));
        m_context.setStrokeStyle(strokeC);
        m_context.setStrokeWidth(m_currentStrokeWidth);
        m_context.setStrokeCap(BL_STROKE_CAP_POSITION_START, m_strokeCap);
        m_context.setStrokeCap(BL_STROKE_CAP_POSITION_END, m_strokeCap);
        m_context.setStrokeJoin(m_strokeJoin);
        if (!m_dashPattern.empty()) {
            BLArray<double> dashArray;
            dashArray.clear();
            for (double d : m_dashPattern) dashArray.append(d);
            m_context.setStrokeDashArray(dashArray);
            m_context.setStrokeDashOffset(m_dashOffset);
        }
        m_context.strokeRect(rect);
    }
}

void Blend2DRenderer::drawCircle(float x, float y, float radius, const glm::vec4& fillColor, const glm::vec4& strokeColor, float strokeWidth) {
    BLEllipse ellipse(x, y, radius, radius);
    if (m_hasFill) {
        BLRgba32 fillC((uint8_t)(m_currentFillColor.r * 255), (uint8_t)(m_currentFillColor.g * 255), (uint8_t)(m_currentFillColor.b * 255), (uint8_t)(m_currentFillColor.a * 255));
        m_context.setCompOp(BL_COMP_OP_SRC_OVER);
        m_context.fillEllipse(ellipse, fillC);
    }
    if (m_hasStroke && m_currentStrokeWidth > 0.0f) {
        BLRgba32 strokeC((uint8_t)(m_currentStrokeColor.r * 255), (uint8_t)(m_currentStrokeColor.g * 255), (uint8_t)(m_currentStrokeColor.b * 255), (uint8_t)(m_currentStrokeColor.a * 255));
        m_context.setStrokeStyle(strokeC);
        m_context.setStrokeWidth(m_currentStrokeWidth);
        m_context.setStrokeCap(BL_STROKE_CAP_POSITION_START, m_strokeCap);
        m_context.setStrokeCap(BL_STROKE_CAP_POSITION_END, m_strokeCap);
        m_context.setStrokeJoin(m_strokeJoin);
        if (!m_dashPattern.empty()) {
            BLArray<double> dashArray;
            dashArray.clear();
            for (double d : m_dashPattern) dashArray.append(d);
            m_context.setStrokeDashArray(dashArray);
            m_context.setStrokeDashOffset(m_dashOffset);
        }
        m_context.strokeEllipse(ellipse);
    }
}

void Blend2DRenderer::drawEllipse(float x, float y, float width, float height, const glm::vec4& fillColor, const glm::vec4& strokeColor, float strokeWidth) {
    BLEllipse ellipse(x, y, width * 0.5, height * 0.5);
    if (m_hasFill) {
        BLRgba32 fillC((uint8_t)(m_currentFillColor.r * 255), (uint8_t)(m_currentFillColor.g * 255), (uint8_t)(m_currentFillColor.b * 255), (uint8_t)(m_currentFillColor.a * 255));
        m_context.setCompOp(BL_COMP_OP_SRC_OVER);
        m_context.fillEllipse(ellipse, fillC);
    }
    if (m_hasStroke && m_currentStrokeWidth > 0.0f) {
        BLRgba32 strokeC((uint8_t)(m_currentStrokeColor.r * 255), (uint8_t)(m_currentStrokeColor.g * 255), (uint8_t)(m_currentStrokeColor.b * 255), (uint8_t)(m_currentStrokeColor.a * 255));
        m_context.setStrokeStyle(strokeC);
        m_context.setStrokeWidth(m_currentStrokeWidth);
        m_context.setStrokeCap(BL_STROKE_CAP_POSITION_START, m_strokeCap);
        m_context.setStrokeCap(BL_STROKE_CAP_POSITION_END, m_strokeCap);
        m_context.setStrokeJoin(m_strokeJoin);
        if (!m_dashPattern.empty()) {
            BLArray<double> dashArray;
            dashArray.clear();
            for (double d : m_dashPattern) dashArray.append(d);
            m_context.setStrokeDashArray(dashArray);
            m_context.setStrokeDashOffset(m_dashOffset);
        }
        m_context.strokeEllipse(ellipse);
    }
}
void Blend2DRenderer::drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, const glm::vec4& fillColor, const glm::vec4& strokeColor, float strokeWidth) {}
void Blend2DRenderer::drawPolygon(const std::vector<glm::vec2>& points, const glm::vec4& fillColor, const glm::vec4& strokeColor, float strokeWidth) {}

void Blend2DRenderer::beginPath() {}
void Blend2DRenderer::moveTo(float x, float y) {}
void Blend2DRenderer::lineTo(float x, float y) {}
void Blend2DRenderer::curveTo(float cx1, float cy1, float cx2, float cy2, float x, float y) {}
void Blend2DRenderer::closePath() {}

void Blend2DRenderer::setFont(const std::string& fontPath, float size) {}
void Blend2DRenderer::drawText(const std::string& text, float x, float y, const glm::vec4& color) {}
glm::vec2 Blend2DRenderer::getTextBounds(const std::string& text) { return glm::vec2(0.0f); }

void Blend2DRenderer::pushMatrix() {}
void Blend2DRenderer::popMatrix() {}
void Blend2DRenderer::translate(float x, float y) {}
void Blend2DRenderer::rotate(float angle) {}
void Blend2DRenderer::scale(float sx, float sy) {}
void Blend2DRenderer::resetMatrix() {}

bool Blend2DRenderer::saveToFile(const std::string& filename) { return false; }
bool Blend2DRenderer::saveToMemory(std::vector<uint8_t>& data) { return false; } 