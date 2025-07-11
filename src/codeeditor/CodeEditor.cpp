#include "codeeditor/CodeEditor.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "TextEditor.h"

CodeEditor::CodeEditor()
    : m_language("cpp")
    , m_theme("dark")
    , m_fontSize(14.0f)
    , m_autoSave(true)
    , m_lineNumbers(true)
    , m_wordWrap(false)
    , m_isModified(false)
    , m_showToolbar(true)
    , m_showStatusBar(true)
    , m_cursorPos(0)
    , m_selectionStart(0)
    , m_selectionEnd(0)
    , m_editor(std::make_unique<TextEditor>())
{
    loadDefaultTemplate();
    m_editor->SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
    m_editor->SetText(m_defaultTemplate);
}

CodeEditor::~CodeEditor() {
}

void CodeEditor::render() {
    ImGui::BeginChild("CodeEditor", ImVec2(0, 0), true);
    
    if (m_showToolbar) {
        renderToolbar();
    }
    
    renderEditor();
    
    if (m_showStatusBar) {
        renderStatusBar();
    }
    
    ImGui::EndChild();
}

void CodeEditor::renderToolbar() {
    if (ImGui::Button("New")) {
        newSketch();
    }
    ImGui::SameLine();
    if (ImGui::Button("Open")) {
        openSketch();
    }
    ImGui::SameLine();
    if (ImGui::Button("Save")) {
        saveSketch();
    }
    ImGui::SameLine();
    if (ImGui::Button("Run")) {
        if (m_onRunRequested) {
            m_onRunRequested();
        }
    }
    ImGui::SameLine();
    ImGui::Checkbox("Auto Save", &m_autoSave);
    ImGui::SameLine();
    ImGui::Checkbox("Line Numbers", &m_lineNumbers);
    ImGui::SameLine();
    ImGui::Checkbox("Word Wrap", &m_wordWrap);
    
    ImGui::Separator();
}

void CodeEditor::renderEditor() {
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    m_editor->Render("CodeEditor", ImVec2(-1, -1), false, m_lineNumbers);
    if (m_editor->IsTextChanged()) {
        m_isModified = true;
        if (m_onCodeChanged) {
            m_onCodeChanged(m_editor->GetText());
        }
    }
    ImGui::PopStyleVar();
}

void CodeEditor::renderStatusBar() {
    ImGui::Separator();
    ImGui::Text("Language: %s | Theme: %s | Font Size: %.1f", 
                m_language.c_str(), m_theme.c_str(), m_fontSize);
    if (m_isModified) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Modified");
    }
}

void CodeEditor::setCode(const std::string& code) {
    if (m_editor) {
        m_editor->SetText(code);
        m_isModified = false;
    }
}

std::string CodeEditor::getCode() const {
    if (m_editor) {
        return m_editor->GetText();
    }
    return std::string();
}

void CodeEditor::newSketch() {
    if (m_editor) {
        m_editor->SetText(m_defaultTemplate);
        m_currentFile = "";
        m_isModified = false;
    }
}

void CodeEditor::openSketch() {
    // In a real implementation, this would open a file dialog
    // For now, we'll just load a sample sketch
    if (m_editor) {
        m_editor->SetText(m_processingTemplate);
        m_currentFile = "sketch.cpp";
        m_isModified = false;
    }
}

void CodeEditor::loadOpenFrameworksTemplate() {
    if (m_editor) {
        m_editor->SetText(m_openframeworksTemplate);
        m_currentFile = "openframeworks_sketch.cpp";
        m_isModified = false;
    }
}

void CodeEditor::saveSketch() {
    // In a real implementation, this would save to a file
    m_isModified = false;
}

void CodeEditor::loadDefaultTemplate() {
    m_defaultTemplate = R"(
// Blot Creative Coding Sketch
// Inspired by DrawBot and Processing

#include "blot.h"

void setup() {
    size(800, 600);
    background(255);
}

void draw() {
    // Your creative coding code goes here
    fill(255, 0, 0);
    ellipse(mouseX, mouseY, 50, 50);
}
)";

    m_processingTemplate = R"(
// Processing-style sketch for Blot

void setup() {
    size(800, 600);
    background(255);
    stroke(0);
    strokeWeight(2);
}

void draw() {
    // Animation loop
    if (mousePressed) {
        fill(255, 0, 0);
        ellipse(mouseX, mouseY, 20, 20);
    }
    
    // Draw a moving circle
    float x = sin(frameCount * 0.05) * 200 + width/2;
    float y = cos(frameCount * 0.03) * 150 + height/2;
    fill(0, 255, 0);
    ellipse(x, y, 30, 30);
}
)";

    m_drawbotTemplate = R"(
// DrawBot-style sketch for Blot

void setup() {
    size(800, 600);
    background(1, 1, 1);
    fill(0, 0, 0);
}

void draw() {
    // DrawBot-style drawing
    for (int i = 0; i < 100; i++) {
        float x = random(width);
        float y = random(height);
        float size = random(10, 50);
        
        fill(random(0, 1), random(0, 1), random(0, 1));
        ellipse(x, y, size, size);
    }
}
)";

    m_openframeworksTemplate = R"(
// OpenFrameworks-style sketch for Blot with addons

#include "bxGui.h"
#include "ofxOsc.h"

// Addon instances
bxGui gui;
ofxOsc osc;

// Parameters
float brightness = 0.5f;
float speed = 1.0f;
bool showGui = true;

void setup() {
    size(800, 600);
    background(0);
    
    // Initialize addons
    gui.setup("Controls");
    osc.setup("localhost", 12345);
    
    // Add GUI controls
    gui.addSlider("brightness", brightness, 0.0f, 1.0f);
    gui.addSlider("speed", speed, 0.1f, 5.0f);
    gui.addToggle("showGui", showGui);
    gui.addButton("sendOSC", [&]() {
        osc.sendMessage("/blot/brightness", brightness);
    });
    
    // Set up OSC callbacks
    osc.onMessageReceived("/blot/parameter", [&](const OscMessage& msg) {
        if (msg.getNumArgs() >= 2) {
            std::string param = msg.getStringArg(0);
            float value = msg.getFloatArg(1);
            if (param == "brightness") brightness = value;
            if (param == "speed") speed = value;
        }
    });
}

void draw() {
    background(0);
    
    // Draw GUI
    if (showGui) {
        gui.begin();
        gui.end();
    }
    
    // Animated circle
    float x = sin(frameCount * speed * 0.05) * 200 + width/2;
    float y = cos(frameCount * speed * 0.03) * 150 + height/2;
    
    fill(brightness * 255);
    ellipse(x, y, 50, 50);
    
    // OSC communication
    if (osc.hasWaitingMessages()) {
        OscMessage msg = osc.getNextMessage();
        // Handle incoming messages
    }
}
)";
    
    m_code = m_defaultTemplate;
}

void CodeEditor::setLanguage(const std::string& language) {
    m_language = language;
    applySyntaxHighlighting();
}

void CodeEditor::setTheme(const std::string& theme) {
    m_theme = theme;
}

void CodeEditor::setFontSize(float size) {
    m_fontSize = size;
}

void CodeEditor::setOnCodeChanged(std::function<void(const std::string&)> callback) {
    m_onCodeChanged = callback;
}

void CodeEditor::setOnRunRequested(std::function<void()> callback) {
    m_onRunRequested = callback;
}

void CodeEditor::applySyntaxHighlighting() {
    // Simple syntax highlighting for C++/creative coding
    m_tokens.clear();
    
    // This is a simplified tokenizer
    // In a real implementation, you'd want a more sophisticated parser
    std::istringstream iss(m_code);
    std::string line;
    int lineNum = 0;
    
    while (std::getline(iss, line)) {
        // Simple keyword detection
        if (line.find("void") != std::string::npos ||
            line.find("int") != std::string::npos ||
            line.find("float") != std::string::npos ||
            line.find("bool") != std::string::npos) {
            // Keywords
        }
        
        if (line.find("setup") != std::string::npos ||
            line.find("draw") != std::string::npos ||
            line.find("size") != std::string::npos ||
            line.find("background") != std::string::npos ||
            line.find("fill") != std::string::npos ||
            line.find("stroke") != std::string::npos ||
            line.find("ellipse") != std::string::npos ||
            line.find("rect") != std::string::npos) {
            // Creative coding functions
        }
        
        lineNum++;
    }
} 