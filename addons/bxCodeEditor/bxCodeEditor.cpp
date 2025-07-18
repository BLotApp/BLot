#include "bxCodeEditor.h"
#include "third_party/ImGuiColorTextEdit/TextEditor.h"

bxCodeEditor::bxCodeEditor()
    : AddonBase("bxCodeEditor")
    , m_editor(std::make_unique<TextEditor>()) {
    m_editor->SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
}

bxCodeEditor::~bxCodeEditor() = default;

void bxCodeEditor::loadDefaultTemplate() {
    std::string defaultCode = R"(
// Blot Creative Coding Sketch

void setup() {
    size(800, 600);
    background(255);
}

void draw() {
    // Your code here
    fill(255, 0, 0);
    ellipse(mouseX, mouseY, 50, 50);
}
)";
    setCode(defaultCode);
}

std::string bxCodeEditor::getCode() const {
    if (m_editor) {
        return m_editor->GetText();
    }
    return m_currentCode;
}

void bxCodeEditor::setCode(const std::string& code) {
    m_currentCode = code;
    if (m_editor) {
        m_editor->SetText(code);
    }
} 