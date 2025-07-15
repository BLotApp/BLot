#include "LogWindow.h"
#include <spdlog/sinks/base_sink.h>
#include <spdlog/details/null_mutex.h>
#include <memory>

void LogWindow::addLogMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(m_logMutex);
    m_logBuffer.push_back(message);
    if (m_logBuffer.size() > m_maxLogLines) {
        m_logBuffer.pop_front();
    }
}

std::deque<std::string> LogWindow::getLogBuffer() const {
    std::lock_guard<std::mutex> lock(m_logMutex);
    return m_logBuffer;
}

void LogWindow::render() {
    auto logBuffer = getLogBuffer();
    ImGui::BeginChild("LogScrollRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
    for (const auto& line : logBuffer) {
        ImGui::TextUnformatted(line.c_str());
    }
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();
}

class LogWindowSink : public spdlog::sinks::base_sink<std::mutex> {
public:
    LogWindowSink(LogWindow* logWindow) : m_logWindow(logWindow) {}
protected:
    void sink_it_(const spdlog::details::log_msg& msg) override {
        if (m_logWindow) {
            spdlog::memory_buf_t formatted;
            spdlog::sinks::base_sink<std::mutex>::formatter_->format(msg, formatted);
            m_logWindow->addLogMessage(fmt::to_string(formatted));
        }
    }
    void flush_() override {}
private:
    LogWindow* m_logWindow;
};

void LogWindow::setupSpdlogSink() {
    auto sink = std::make_shared<LogWindowSink>(this);
    setSpdlogSink(sink);
    spdlog::default_logger()->sinks().push_back(sink);
} 