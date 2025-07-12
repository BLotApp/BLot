#pragma once

#include "Window.h"
#include <imgui.h>
#include <string>
#include <vector>

namespace blot {

class TerminalWindow : public Window {
public:
    TerminalWindow(const std::string& title = "Terminal###Terminal", 
                   Flags flags = Flags::None);
    virtual ~TerminalWindow() = default;

    // Terminal functionality
    void addLog(const std::string& message);
    void clearLog();
    void executeCommand(const std::string& command);
    
    // Rendering
    virtual void render() override;

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

private:
    std::vector<std::string> m_logHistory;
    char m_inputBuffer[1024] = {0};
    bool m_scrollToBottom = true;
    
    // Terminal methods
    void renderInput();
    void renderLogHistory();
    void processCommand(const std::string& command);
};

} // namespace blot 