#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>
#include "rendering/IRenderer.h"

namespace blot {

class Graphics {
  public:
	Graphics();
	~Graphics();
	void setRenderer(IRenderer *renderer);
	IRenderer *getRenderer() const { return m_renderer; }

	// Color and style
	void setFillColor(float r, float g, float b, float a = 1.0f);
	void setStrokeColor(float r, float g, float b, float a = 1.0f);
	void setStrokeWidth(float width);
	void setFillOpacity(float opacity);

	// Basic shapes
	void drawRect(float x, float y, float width, float height);
	void drawEllipse(float x, float y, float width, float height);
	void drawCircle(float x, float y, float radius);
	void drawLine(float x1, float y1, float x2, float y2);
	void drawTriangle(float x1, float y1, float x2, float y2, float x3,
					  float y3);
	void drawPolygon(const std::vector<glm::vec2> &points);
	void rect(float x, float y, float width, float height);

	// Path drawing
	void beginPath();
	void moveTo(float x, float y);
	void lineTo(float x, float y);
	void curveTo(float x1, float y1, float x2, float y2, float x3, float y3);
	void closePath();
	void fill();
	void stroke();

	// Text
	void drawText(const std::string &text, float x, float y);
	void setFont(const std::string &fontName, float size);
	void setTextAlign(int align);

	// Transformations
	void pushMatrix();
	void popMatrix();
	void translate(float x, float y);
	void rotate(float angle);
	void scale(float x, float y);
	void transform(float a, float b, float c, float d, float e, float f);

	// Effects and filters
	void setBlendMode(int mode);
	void setShadow(float x, float y, float blur, float r, float g, float b,
				   float a = 1.0f);
	void setGradient(float x1, float y1, float r1, float g1, float b1, float x2,
					 float y2, float r2, float g2, float b2);
	void setStrokeCap(int cap);
	void setStrokeJoin(int join);
	void setStrokeDash(const std::vector<float> &dashes, float offset = 0.0f);

	// Image operations
	void drawImage(const std::string &imagePath, float x, float y,
				   float width = 0, float height = 0);
	void setImageMode(int mode);

	// Utility
	void clear(float r, float g, float b, float a = 1.0f);
	void save();
	void restore();
	void setCanvasSize(int width, int height);

	// Getters
	glm::vec4 getFillColor() const { return m_fillColor; }
	glm::vec4 getStrokeColor() const { return m_strokeColor; }
	float getStrokeWidth() const { return m_strokeWidth; }

  private:
	void initShaders();
	void updateTransform();

	// PIMPL for OpenGL resources
	struct Impl;
	std::unique_ptr<Impl> m_impl;

	// Drawing state
	glm::vec4 m_fillColor;
	glm::vec4 m_strokeColor;
	float m_strokeWidth;
	float m_fillOpacity;

	// Transform state
	std::vector<glm::mat4> m_matrixStack;
	glm::mat4 m_currentMatrix;

	// Path state
	std::vector<glm::vec2> m_currentPath;
	bool m_pathOpen;

	// Text state
	std::string m_currentFont;
	float m_fontSize;
	int m_textAlign;

	// Effects
	int m_blendMode;
	bool m_hasShadow;
	glm::vec4 m_shadowColor;
	glm::vec2 m_shadowOffset;
	float m_shadowBlur;

	// Gradient
	bool m_hasGradient;
	glm::vec2 m_gradientStart;
	glm::vec2 m_gradientEnd;
	glm::vec4 m_gradientStartColor;
	glm::vec4 m_gradientEndColor;

	IRenderer *m_renderer = nullptr;
	int m_canvasWidth = 0;
	int m_canvasHeight = 0;
};

} // namespace blot
