#include "Logger.h"
#include "../../imgui/imgui.h"

void RenderConsoleUI(std::vector<LogEntry> &logBuffer)
{
    static bool autoScroll = true;
    ImGui::Begin("Console");

    if (ImGui::Button("Clear"))
        logBuffer.clear();
    ImGui::SameLine();
    ImGui::Checkbox("Auto-scroll", &autoScroll);
    ImGui::Separator();

    ImGui::BeginChild("ConsoleScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    for (const auto &entry : logBuffer)
    {
        if (entry.level == LogLevel::Warning)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 180, 50, 255));
            ImGui::TextUnformatted(entry.message.c_str());
            ImGui::PopStyleColor();
        }
        else if (entry.level == LogLevel::Error)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 60, 60, 255));
            ImGui::TextUnformatted(entry.message.c_str());
            ImGui::PopStyleColor();
        }
        else
        {
            ImGui::TextUnformatted(entry.message.c_str());
        }
    }
    if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 5.0f)
        ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();

    ImGui::End();
}
