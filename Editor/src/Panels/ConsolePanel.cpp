#include "Panels/ConsolePanel.hpp"

#include <imgui.h>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace Mixture
{
    std::vector<LogMessage> ConsolePanel::s_Logs;
    std::mutex ConsolePanel::s_LogMutex;

    ConsolePanel::ConsolePanel()
        : IEditorPanel("Console", true)
    {}

    // TODO: integrate with spdlog
    void ConsolePanel::AddLog(LogLevel level, const std::string& message)
    {
        std::lock_guard<std::mutex> lock(s_LogMutex);

        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%H:%M:%S");

        s_Logs.push_back({ level, message, ss.str() });
    }

    void ConsolePanel::ClearLogs()
    {
        std::lock_guard<std::mutex> lock(s_LogMutex);
        s_Logs.clear();
    }

    void ConsolePanel::OnDrawImGui()
    {
        ImGui::Begin(m_Name.c_str(), &m_IsOpen);

        // Toolbar Controls
        if (ImGui::Button("Clear")) ClearLogs();
        ImGui::SameLine();
        ImGui::Checkbox("Auto-Scroll", &m_AutoScroll);
        ImGui::SameLine(0.0f, 20.0f);

        uint32_t infoCount = 0, warnCount = 0, errCount = 0;
        {
            std::lock_guard<std::mutex> lock(s_LogMutex);
            for (const auto& log : s_Logs)
            {
                if (log.Level == LogLevel::Info) infoCount++;
                else if (log.Level == LogLevel::Warning) warnCount++;
                else if (log.Level == LogLevel::Error) errCount++;
            }
        }

        char infoLabel[32], warnLabel[32], errLabel[32];
        snprintf(infoLabel, sizeof(infoLabel), "Info (%u)", infoCount);
        snprintf(warnLabel, sizeof(warnLabel), "Warn (%u)", warnCount);
        snprintf(errLabel, sizeof(errLabel), "Error (%u)", errCount);

        ImGui::Checkbox(infoLabel, &m_ShowInfo); ImGui::SameLine();
        ImGui::Checkbox(warnLabel, &m_ShowWarnings); ImGui::SameLine();
        ImGui::Checkbox(errLabel, &m_ShowErrors); ImGui::SameLine(0.0f, 20.0f);

        ImGui::SetNextItemWidth(200);
        ImGui::InputText("Filter", m_FilterBuffer, sizeof(m_FilterBuffer));

        ImGui::Separator();

        // Log Window Scroll Region
        if (ImGui::BeginChild("LogScrollRegion", ImVec2(0, 0), ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar))
        {
            std::lock_guard<std::mutex> lock(s_LogMutex);
            for (const auto& log : s_Logs)
            {
                if (log.Level == LogLevel::Info && !m_ShowInfo) continue;
                if (log.Level == LogLevel::Warning && !m_ShowWarnings) continue;
                if (log.Level == LogLevel::Error && !m_ShowErrors) continue;

                if (m_FilterBuffer[0] != '\0' && log.Message.find(m_FilterBuffer) == std::string::npos)
                    continue;

                ImVec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
                if (log.Level == LogLevel::Warning) color = ImVec4{ 1.0f, 0.8f, 0.2f, 1.0f };
                else if (log.Level == LogLevel::Error) color = ImVec4{ 1.0f, 0.3f, 0.3f, 1.0f };

                ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::Text("[%s] %s", log.Timestamp.c_str(), log.Message.c_str());
                ImGui::PopStyleColor();
            }

            if (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();

        ImGui::End();
    }
}
