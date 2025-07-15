#include <deque>
#include <mutex>
#include <memory>
namespace spdlog { class logger; namespace sinks { class sink; } }

public:
    void addLogMessage(const std::string& message);
    std::deque<std::string> getLogBuffer() const;
    void setupSpdlogSink();
    // For spdlog sink integration
    void setSpdlogSink(std::shared_ptr<spdlog::sinks::sink> sink) { m_spdlogSink = sink; }
    std::shared_ptr<spdlog::sinks::sink> getSpdlogSink() const { return m_spdlogSink; }

private:
    std::deque<std::string> m_logBuffer;
    std::mutex m_logMutex;
    size_t m_maxLogLines = 1000;
    std::shared_ptr<spdlog::sinks::sink> m_spdlogSink; 