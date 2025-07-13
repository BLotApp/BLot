#pragma once

#include "Window.h"
#include <imgui.h>
#include <string>
#include <vector>
#include <mutex>

namespace blot {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

struct LogEntry {
    LogLevel level;
    std::string message;
    std::string timestamp;
    
    LogEntry(LogLevel lvl, const std::string& msg, const std::string& time = "")
        : level(lvl), message(msg), timestamp(time) {}
};

class LogWindow : public Window {
public:
    LogWindow(const std::string& title = "Log###Log", 
              Flags flags = Flags::None);
    virtual ~LogWindow() = default;

    // Log functionality
    void addLog(LogLevel level, const std::string& message);
    void addDebug(const std::string& message);
    void addInfo(const std::string& message);
    void addWarning(const std::string& message);
    void addError(const std::string& message);
    void clearLog();
    
    // Rendering
    virtual void render() override;

private:
    std::vector<LogEntry> m_logEntries;
    std::mutex m_logMutex;
    bool m_scrollToBottom = true;
    bool m_showDebug = true;
    bool m_showInfo = true;
    bool m_showWarning = true;
    bool m_showError = true;
    
    // Log methods
    void renderLogEntries();
    void renderFilterControls();
    ImVec4 getLogColor(LogLevel level);
    std::string getLogLevelString(LogLevel level);
    std::string getCurrentTimestamp();
};

} // namespace blot 