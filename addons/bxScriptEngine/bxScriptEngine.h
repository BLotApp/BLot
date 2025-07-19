#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace blot {
class Canvas;
class Graphics;
} // namespace blot

class bxScriptEngine {
  public:
	bxScriptEngine();
	~bxScriptEngine();

	void runCode(const std::string &code);
	void stop();
	void update(float deltaTime);
	// Convenience wrapper used by examples
	void updateScript(float deltaTime) { update(deltaTime); }

	// API functions that can be called from scripts
	void size(int width, int height);
	void background(float r, float g, float b, float a = 1.0f);
	void fill(float r, float g, float b, float a = 1.0f);
	void noFill();
	void stroke(float r, float g, float b, float a = 1.0f);
	void noStroke();
	void strokeWeight(float weight);
	void strokeCap(int cap);
	void strokeJoin(int join);
	void strokeDash(const std::vector<float> &dashes, float offset = 0.0f);

	void rect(float x, float y, float width, float height);
	void ellipse(float x, float y, float width, float height);
	void line(float x1, float y1, float x2, float y2);
	void triangle(float x1, float y1, float x2, float y2, float x3, float y3);
	void circle(float x, float y, float diameter);

	void text(const std::string &text, float x, float y);
	void textSize(float size);
	void textAlign(int align);

	void pushMatrix();
	void popMatrix();
	void translate(float x, float y);
	void rotate(float angle);
	void scale(float x, float y);

	// Mouse and keyboard state
	float mouseX() const;
	float mouseY() const;
	bool mousePressed() const;
	bool keyPressed() const;
	char key() const;

	// Animation state
	int frameCount() const;
	float frameRate() const;
	void frameRate(float fps);

	// Math functions
	float random(float min, float max);
	float sin(float angle);
	float cos(float angle);
	float map(float value, float start1, float stop1, float start2,
			  float stop2);

	// Callbacks
	void setOnSetup(std::function<void()> callback);
	void setOnDraw(std::function<void()> callback);
	void setOnMousePressed(std::function<void()> callback);
	void setOnMouseReleased(std::function<void()> callback);
	void setOnKeyPressed(std::function<void()> callback);

	// Canvas integration
	void setCanvas(std::shared_ptr<blot::Canvas> canvas);
	void setGraphics(std::shared_ptr<blot::Graphics> graphics);

  private:
	void initAPI();
	void executeSetup();
	void executeDraw();
	void handleInput();

	// Script state
	std::string m_currentCode;
	bool m_isRunning;
	bool m_hasSetup;
	bool m_hasDraw;

	// Canvas and graphics
	std::shared_ptr<blot::Canvas> m_canvas;
	std::shared_ptr<blot::Graphics> m_graphics;

	// Animation state
	int m_frameCount;
	float m_frameRate;
	float m_lastFrameTime;
	float m_time;

	// Input state
	float m_mouseX;
	float m_mouseY;
	bool m_mousePressed;
	bool m_keyPressed;
	char m_currentKey;

	// Canvas state
	int m_width;
	int m_height;

	// Drawing state
	bool m_hasFill;
	bool m_hasStroke;
	float m_fillR, m_fillG, m_fillB, m_fillA;
	float m_strokeR, m_strokeG, m_strokeB, m_strokeA;
	float m_strokeWeight;
	float m_textSize;
	int m_textAlign;

	// Transform stack
	std::vector<glm::mat4> m_matrixStack;
	glm::mat4 m_currentMatrix;

	// Callbacks
	std::function<void()> m_onSetup;
	std::function<void()> m_onDraw;
	std::function<void()> m_onMousePressed;
	std::function<void()> m_onMouseReleased;
	std::function<void()> m_onKeyPressed;

	// API functions map
	std::unordered_map<std::string, std::function<void()>> m_apiFunctions;
};
