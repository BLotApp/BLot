#pragma once

#include <string>
#include <vector>
#include <memory>
#include "imgui.h"
#include <functional>

class CodeEditor {
public:
    CodeEditor();
    ~CodeEditor();
    
    void render();
    void setCode(const std::string& code);
    std::string getCode() const;
    
    // File operations
    void newSketch();
    void openSketch();
    void saveSketch();
    void loadDefaultTemplate();
    void loadOpenFrameworksTemplate();
    
    // Editor features
    void setLanguage(const std::string& language);
    void setTheme(const std::string& theme);
    void setFontSize(float size);
    
    // Callbacks
    void setOnCodeChanged(std::function<void(const std::string&)> callback);
    void setOnRunRequested(std::function<void()> callback);
    
private:
    void renderToolbar();
    void renderEditor();
    void renderStatusBar();
    void handleInput();
    void applySyntaxHighlighting();
    
    // Editor state
    std::string m_code;
    std::string m_language;
    std::string m_theme;
    float m_fontSize;
    bool m_autoSave;
    bool m_lineNumbers;
    bool m_wordWrap;
    
    // File state
    std::string m_currentFile;
    bool m_isModified;
    
    // Editor features
    bool m_showToolbar;
    bool m_showStatusBar;
    int m_cursorPos;
    int m_selectionStart;
    int m_selectionEnd;
    
    // Callbacks
    std::function<void(const std::string&)> m_onCodeChanged;
    std::function<void()> m_onRunRequested;
    
    // Syntax highlighting
    struct Token {
        std::string text;
        std::string type;
        int start;
        int end;
    };
    std::vector<Token> m_tokens;
    
    // Default templates
    std::string m_defaultTemplate;
    std::string m_processingTemplate;
    std::string m_drawbotTemplate;
    std::string m_openframeworksTemplate;
}; 