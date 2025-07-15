#include "rendering/Blend2DRenderer.h"
#include <iostream>
#include <blend2d.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <spdlog/spdlog.h>

Blend2DRenderer::Blend2DRenderer() : m_initialized(false), m_width(0), m_height(0) {
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
    if (m_context) m_context.end();
    m_image.create(width, height, BL_FORMAT_PRGB32);
    m_context.begin(m_image);
    m_context.setCompOp(BL_COMP_OP_SRC_COPY);
    m_context.fillAll(BLRgba32(0xFFFFFFFF));
    m_context.setCompOp(BL_COMP_OP_SRC_OVER);
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

void Blend2DRenderer::beginFrame() {
    spdlog::info("[Blend2DRenderer] Begin Frame");
}
void Blend2DRenderer::endFrame() {
    spdlog::info("[Blend2DRenderer] End Frame");
}

void Blend2DRenderer::clear(const glm::vec4& color) {
    BLRgba32 c((uint8_t)(color.r * 255), (uint8_t)(color.g * 255), (uint8_t)(color.b * 255), (uint8_t)(color.a * 255));
    m_context.setCompOp(BL_COMP_OP_SRC_COPY);
    m_context.fillAll(c);
}

void Blend2DRenderer::drawLine(float x1, float y1, float x2, float y2) {
    if (!m_hasStroke || m_strokeWidth <= 0.0f) return;
    
    BLRgba32 strokeC(
        uint8_t(m_strokeColor.r * 255),
        uint8_t(m_strokeColor.g * 255),
        uint8_t(m_strokeColor.b * 255),
        uint8_t(m_strokeColor.a * 255)
    );
    
    m_context.setStrokeStyle(strokeC);
    updateStrokeOptions();
    m_context.strokeLine(BLPoint(x1, y1), BLPoint(x2, y2));
}

void Blend2DRenderer::fill(const glm::vec4& color) {
    m_currentFillColor = color;
}

void Blend2DRenderer::stroke(const glm::vec4& color, float width) {
    m_currentStrokeColor = color;
    m_currentStrokeWidth = width;
}

void Blend2DRenderer::noFill() {
    m_hasFill = false;
}

void Blend2DRenderer::noStroke() {
    m_hasStroke = false;
}

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

void Blend2DRenderer::setStrokeDashArray(const std::vector<double>& dashes) {
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

void Blend2DRenderer::setStrokeOptions(const BLStrokeOptions& options) {
    m_strokeOptions = options;
}

void Blend2DRenderer::setStrokeDashPattern(const std::vector<double>& dashes, double offset) {
    setStrokeDashArray(dashes);
    setStrokeDashOffset(offset);
}

void Blend2DRenderer::updateStrokeOptions() {
    m_strokeOptions.width = m_strokeWidth;
    m_context.setStrokeWidth(m_strokeOptions.width);
    m_context.setStrokeCap(BL_STROKE_CAP_POSITION_START, static_cast<BLStrokeCap>(m_strokeOptions.startCap));
    m_context.setStrokeCap(BL_STROKE_CAP_POSITION_END, static_cast<BLStrokeCap>(m_strokeOptions.endCap));
    m_context.setStrokeJoin(static_cast<BLStrokeJoin>(m_strokeOptions.join));
    m_context.setStrokeMiterLimit(m_strokeOptions.miterLimit);
    m_context.setStrokeDashOffset(m_strokeOptions.dashOffset);
    m_context.setStrokeTransformOrder(static_cast<BLStrokeTransformOrder>(m_strokeOptions.transformOrder));
    
    // Set dash array if not empty
    if (m_strokeOptions.dashArray.size() > 0) {
        m_context.setStrokeDashArray(m_strokeOptions.dashArray);
    } else {
        // Clear dash array
        BLArray<double> emptyArray;
        m_context.setStrokeDashArray(emptyArray);
    }
}

void Blend2DRenderer::setFillColor(const glm::vec4& color) {
    m_fillColor = color;
}

void Blend2DRenderer::setStrokeColor(const glm::vec4& color) {
    m_strokeColor = color;
}

void Blend2DRenderer::setStrokeWidth(float width) {
    m_strokeWidth = width;
}

void Blend2DRenderer::drawRect(float x, float y, float width, float height) {
    BLRgba32 fill(
        uint8_t(m_fillColor.r * 255),
        uint8_t(m_fillColor.g * 255),
        uint8_t(m_fillColor.b * 255),
        uint8_t(m_fillColor.a * 255)
    );
    BLRgba32 stroke(
        uint8_t(m_strokeColor.r * 255),
        uint8_t(m_strokeColor.g * 255),
        uint8_t(m_strokeColor.b * 255),
        uint8_t(m_strokeColor.a * 255)
    );
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
            uint8_t(m_fillColor.r * 255),
            uint8_t(m_fillColor.g * 255),
            uint8_t(m_fillColor.b * 255),
            uint8_t(m_fillColor.a * 255)
        );
        m_context.setCompOp(BL_COMP_OP_SRC_OVER);
        m_context.setFillStyle(fillC);
        m_context.fillEllipse(ellipse);
    }
    if (m_hasStroke && m_strokeWidth > 0.0f) {
        BLRgba32 strokeC(
            uint8_t(m_strokeColor.r * 255),
            uint8_t(m_strokeColor.g * 255),
            uint8_t(m_strokeColor.b * 255),
            uint8_t(m_strokeColor.a * 255)
        );
        m_context.setStrokeStyle(strokeC);
        updateStrokeOptions();
        m_context.strokeEllipse(ellipse);
    }
}

void Blend2DRenderer::drawEllipse(float x, float y, float width, float height) {
    BLEllipse ellipse(x, y, width * 0.5, height * 0.5);
    if (m_hasFill) {
        BLRgba32 fillC(
            uint8_t(m_fillColor.r * 255),
            uint8_t(m_fillColor.g * 255),
            uint8_t(m_fillColor.b * 255),
            uint8_t(m_fillColor.a * 255)
        );
        m_context.setCompOp(BL_COMP_OP_SRC_OVER);
        m_context.setFillStyle(fillC);
        m_context.fillEllipse(ellipse);
    }
    if (m_hasStroke && m_strokeWidth > 0.0f) {
        BLRgba32 strokeC(
            uint8_t(m_strokeColor.r * 255),
            uint8_t(m_strokeColor.g * 255),
            uint8_t(m_strokeColor.b * 255),
            uint8_t(m_strokeColor.a * 255)
        );
        m_context.setStrokeStyle(strokeC);
        updateStrokeOptions();
        m_context.strokeEllipse(ellipse);
    }
}

void Blend2DRenderer::drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3) {
    BLPath path;
    path.moveTo(x1, y1);
    path.lineTo(x2, y2);
    path.lineTo(x3, y3);
    path.close();
    if (m_hasFill) {
        BLRgba32 fillC(
            uint8_t(m_fillColor.r * 255),
            uint8_t(m_fillColor.g * 255),
            uint8_t(m_fillColor.b * 255),
            uint8_t(m_fillColor.a * 255)
        );
        m_context.setCompOp(BL_COMP_OP_SRC_OVER);
        m_context.setFillStyle(fillC);
        m_context.fillPath(path);
    }
    if (m_hasStroke && m_strokeWidth > 0.0f) {
        BLRgba32 strokeC(
            uint8_t(m_strokeColor.r * 255),
            uint8_t(m_strokeColor.g * 255),
            uint8_t(m_strokeColor.b * 255),
            uint8_t(m_strokeColor.a * 255)
        );
        m_context.setStrokeStyle(strokeC);
        updateStrokeOptions();
        m_context.strokePath(path);
    }
}

void Blend2DRenderer::drawPolygon(const std::vector<glm::vec2>& points) {
    if (points.size() < 3) return;
    BLPath path;
    path.moveTo(points[0].x, points[0].y);
    for (size_t i = 1; i < points.size(); ++i) {
        path.lineTo(points[i].x, points[i].y);
    }
    path.close();
    if (m_hasFill) {
        BLRgba32 fillC(
            uint8_t(m_fillColor.r * 255),
            uint8_t(m_fillColor.g * 255),
            uint8_t(m_fillColor.b * 255),
            uint8_t(m_fillColor.a * 255)
        );
        m_context.setCompOp(BL_COMP_OP_SRC_OVER);
        m_context.setFillStyle(fillC);
        m_context.fillPath(path);
    }
    if (m_hasStroke && m_strokeWidth > 0.0f) {
        BLRgba32 strokeC(
            uint8_t(m_strokeColor.r * 255),
            uint8_t(m_strokeColor.g * 255),
            uint8_t(m_strokeColor.b * 255),
            uint8_t(m_strokeColor.a * 255)
        );
        m_context.setStrokeStyle(strokeC);
        updateStrokeOptions();
        m_context.strokePath(path);
    }
}

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

void Blend2DRenderer::flush() {
    m_context.end();
    m_context.begin(m_image);
} 