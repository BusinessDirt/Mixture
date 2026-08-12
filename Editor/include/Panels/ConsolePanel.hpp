#pragma once

#include "Panels/IEditorPanel.hpp"
#include <vector>
#include <string>
#include <mutex>

namespace Mixture
{
    enum class LogLevel
    {
        Info,
        Warning,
        Error
    };

    struct LogMessage
    {
        LogLevel Level = LogLevel::Info;
        std::string Message;
        std::string Timestamp;
    };

    /**
     * @brief Panel responsible for displaying real-time engine logs and diagnostics.
     */
    class ConsolePanel final : public IEditorPanel
    {
    public:
        ConsolePanel();
        ~ConsolePanel() override = default;

        void OnDrawImGui() override;

        /** Adds a log message to the console panel. */
        static void AddLog(LogLevel level, const std::string& message);

        /** Clears all log entries. */
        void ClearLogs();

    private:
        static std::vector<LogMessage> s_Logs;
        static std::mutex s_LogMutex;

        bool m_ShowInfo = true;
        bool m_ShowWarnings = true;
        bool m_ShowErrors = true;
        bool m_AutoScroll = true;
        char m_FilterBuffer[256]{};
    };
}
