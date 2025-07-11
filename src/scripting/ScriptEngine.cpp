#include "scripting/ScriptEngine.h"
#include "rendering/Graphics.h"
#include "canvas/Canvas.h"
#include <iostream>
#include <random>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

ScriptEngine::ScriptEngine()
    : m_isRunning(false)
    , m_hasSetup(false)
    , m_hasDraw(false)
    , m_frameCount(0)
    , m_frameRate(60.0f)
    , m_lastFrameTime(0.0f)
    , m_time(0.0f)
    , m_mouseX(0.0f)
    , m_mouseY(0.0f)
    , m_mousePressed(false)
    , m_keyPressed(false)
    , m_currentKey(0)
    , m_width(800)
    , m_height(600)
    , m_hasFill(true)
    , m_hasStroke(true)
    , m_fillR(1.0f), m_fillG(1.0f), m_fillB(1.0f), m_fillA(1.0f)
    , m_strokeR(0.0f), m_strokeG(0.0f), m_strokeB(0.0f), m_strokeA(1.0f)
    , m_strokeWeight(1.0f)
    , m_textSize(12.0f)
    , m_textAlign(0)
{
    m_currentMatrix = glm::mat4(1.0f);
    initAPI();
}

ScriptEngine::~ScriptEngine() {
}

void ScriptEngine::runCode(const std::string& code) {
    m_currentCode = code;
    m_isRunning = true;
    m_frameCount = 0;
    m_time = 0.0f;
    
    // Reset state
    m_hasSetup = false;
    m_hasDraw = false;
    
    // Parse the code to find setup() and draw() functions
    // This is a simplified parser - in a real implementation, you'd want a proper parser
    if (code.find("void setup()") != std::string::npos) {
        m_hasSetup = true;
    }
    if (code.find("void draw()") != std::string::npos) {
        m_hasDraw = true;
    }
    
    // Execute setup if present
    if (m_hasSetup) {
        executeSetup();
    }
}

void ScriptEngine::stop() {
    m_isRunning = false;
}

void ScriptEngine::update(float deltaTime) {
    if (!m_isRunning) return;
    
    m_time += deltaTime;
    
    // Check if it's time for the next frame
    if (m_time - m_lastFrameTime >= 1.0f / m_frameRate) {
        m_frameCount++;
        m_lastFrameTime = m_time;
        
        if (m_hasDraw) {
            executeDraw();
        }
    }
}

void ScriptEngine::size(int width, int height) {
    m_width = width;
    m_height = height;
    if (m_canvas) {
        m_canvas->resize(width, height);
    }
}

void ScriptEngine::background(float r, float g, float b, float a) {
    if (m_canvas) {
        m_canvas->background(r, g, b, a);
    }
}

void ScriptEngine::fill(float r, float g, float b, float a) {
    m_fillR = r;
    m_fillG = g;
    m_fillB = b;
    m_fillA = a;
    m_hasFill = true;
}

void ScriptEngine::noFill() {
    m_hasFill = false;
}

void ScriptEngine::stroke(float r, float g, float b, float a) {
    m_strokeR = r;
    m_strokeG = g;
    m_strokeB = b;
    m_strokeA = a;
    m_hasStroke = true;
}

void ScriptEngine::noStroke() {
    m_hasStroke = false;
}

void ScriptEngine::strokeWeight(float weight) {
    m_strokeWeight = weight;
}

void ScriptEngine::rect(float x, float y, float width, float height) {
    if (m_canvas) {
        m_canvas->rect(x, y, width, height);
    }
}

void ScriptEngine::ellipse(float x, float y, float width, float height) {
    if (m_canvas) {
        m_canvas->ellipse(x, y, width, height);
    }
}

void ScriptEngine::line(float x1, float y1, float x2, float y2) {
    if (m_canvas) {
        m_canvas->line(x1, y1, x2, y2);
    }
}

void ScriptEngine::triangle(float x1, float y1, float x2, float y2, float x3, float y3) {
    if (m_canvas) {
        m_canvas->triangle(x1, y1, x2, y2, x3, y3);
    }
}

void ScriptEngine::circle(float x, float y, float diameter) {
    if (m_canvas) {
        m_canvas->circle(x, y, diameter);
    }
}

void ScriptEngine::text(const std::string& text, float x, float y) {
    if (m_canvas) {
        m_canvas->text(text, x, y);
    }
}

void ScriptEngine::textSize(float size) {
    m_textSize = size;
}

void ScriptEngine::textAlign(int align) {
    m_textAlign = align;
}

void ScriptEngine::pushMatrix() {
    m_matrixStack.push_back(m_currentMatrix);
}

void ScriptEngine::popMatrix() {
    if (!m_matrixStack.empty()) {
        m_currentMatrix = m_matrixStack.back();
        m_matrixStack.pop_back();
    }
}

void ScriptEngine::translate(float x, float y) {
    m_currentMatrix = glm::translate(m_currentMatrix, glm::vec3(x, y, 0.0f));
}

void ScriptEngine::rotate(float angle) {
    m_currentMatrix = glm::rotate(m_currentMatrix, angle, glm::vec3(0.0f, 0.0f, 1.0f));
}

void ScriptEngine::scale(float x, float y) {
    m_currentMatrix = glm::scale(m_currentMatrix, glm::vec3(x, y, 1.0f));
}

float ScriptEngine::mouseX() const {
    return m_mouseX;
}

float ScriptEngine::mouseY() const {
    return m_mouseY;
}

bool ScriptEngine::mousePressed() const {
    return m_mousePressed;
}

bool ScriptEngine::keyPressed() const {
    return m_keyPressed;
}

char ScriptEngine::key() const {
    return m_currentKey;
}

int ScriptEngine::frameCount() const {
    return m_frameCount;
}

float ScriptEngine::frameRate() const {
    return m_frameRate;
}

void ScriptEngine::frameRate(float fps) {
    m_frameRate = fps;
}

float ScriptEngine::random(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

float ScriptEngine::sin(float angle) {
    return std::sin(angle);
}

float ScriptEngine::cos(float angle) {
    return std::cos(angle);
}

float ScriptEngine::map(float value, float start1, float stop1, float start2, float stop2) {
    return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}

void ScriptEngine::setOnSetup(std::function<void()> callback) {
    m_onSetup = callback;
}

void ScriptEngine::setOnDraw(std::function<void()> callback) {
    m_onDraw = callback;
}

void ScriptEngine::setOnMousePressed(std::function<void()> callback) {
    m_onMousePressed = callback;
}

void ScriptEngine::setOnMouseReleased(std::function<void()> callback) {
    m_onMouseReleased = callback;
}

void ScriptEngine::setOnKeyPressed(std::function<void()> callback) {
    m_onKeyPressed = callback;
}

void ScriptEngine::setCanvas(std::shared_ptr<Canvas> canvas) {
    m_canvas = canvas;
}

void ScriptEngine::setGraphics(std::shared_ptr<Graphics> graphics) {
    m_graphics = graphics;
}

void ScriptEngine::initAPI() {
    // Initialize the API functions map
    // This would be used for dynamic function calling
}

void ScriptEngine::executeSetup() {
    if (m_onSetup) {
        m_onSetup();
    }
}

void ScriptEngine::executeDraw() {
    if (m_onDraw) {
        m_onDraw();
    }
}

void ScriptEngine::handleInput() {
    // Handle mouse and keyboard input
    // This would be called from the main application loop
} 

void ScriptEngine::strokeCap(int cap) {
    if (m_canvas) m_canvas->strokeCap(cap);
}
void ScriptEngine::strokeJoin(int join) {
    if (m_canvas) m_canvas->strokeJoin(join);
}
void ScriptEngine::strokeDash(const std::vector<float>& dashes, float offset) {
    if (m_canvas) m_canvas->strokeDash(dashes, offset);
} 