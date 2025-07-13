#pragma once

#include <string>
#include <memory>

// Forward declaration
class TextEditor;

class CodeEditor {
public:
    CodeEditor();
    ~CodeEditor();
    
    void loadDefaultTemplate();
    std::string getCode() const;
    void setCode(const std::string& code);
    
private:
    std::unique_ptr<TextEditor> m_editor;
    std::string m_currentCode;
}; 