#pragma once

#include "Window.h"
#include <memory>

// Forward declaration
class CodeEditor;

namespace blot {

class CodeEditorWindow : public Window {
public:
    CodeEditorWindow(const std::string& title, Window::Flags flags = Window::Flags::None);
    ~CodeEditorWindow() = default;

    void setCodeEditor(std::shared_ptr<CodeEditor> codeEditor) { m_codeEditor = codeEditor; }

    // Window interface overrides
    void show() override;
    void hide() override;
    void close() override;
    void toggle() override;
    bool isOpen() const override;
    bool isVisible() const override;
    bool isFocused() const override;
    bool isHovered() const override;
    bool isDragging() const override;
    bool isResizing() const override;
    void setPosition(const ImVec2&) override;
    void setSize(const ImVec2&) override;
    void setMinSize(const ImVec2&) override;
    void setMaxSize(const ImVec2&) override;
    void setFlags(Window::Flags) override;
    int getFlags() const override;
    std::string getTitle() const override;

protected:
    void render() override;

private:
    std::shared_ptr<CodeEditor> m_codeEditor;
};

} // namespace blot 