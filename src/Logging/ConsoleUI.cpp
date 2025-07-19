#include "Logger.h"
#include "../../imgui/imgui.h"
#include "../../imgui/textselect.hpp"

void RenderConsoleUI(std::vector<LogEntry> &logBuffer)
{
    static bool autoScroll = true;

    auto getLine = [&logBuffer](size_t idx) -> std::string_view
    {
        return logBuffer[idx].message;
    };

    auto getNumLines = [&logBuffer]() -> size_t
    {
        return logBuffer.size();
    };

    static TextSelect textSelect(getLine, getNumLines);

    ImGui::Begin("Console");

    if (ImGui::Button("Clear"))
    {
        logBuffer.clear();
    }

    ImGui::SameLine();
    ImGui::Checkbox("Auto-scroll", &autoScroll);
    ImGui::Separator();

    ImGui::BeginChild("ConsoleScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove);

    for (size_t i = 0; i < logBuffer.size(); ++i)
    {
        const auto &entry = logBuffer[i];

        ImU32 color;
        switch (entry.level)
        {
        case LogLevel::Warning:
            color = IM_COL32(255, 180, 50, 255);
            break;
        case LogLevel::Error:
            color = IM_COL32(255, 60, 60, 255);
            break;
        default:
            color = ImGui::GetColorU32(ImGuiCol_Text);
            break;
        }

        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(entry.message.c_str());
        ImGui::PopStyleColor();
    }

    textSelect.update();

    if (ImGui::BeginPopupContextWindow())
    {
        ImGui::BeginDisabled(!textSelect.hasSelection());
        if (ImGui::MenuItem("Copy", "Ctrl+C"))
            textSelect.copy();
        ImGui::EndDisabled();

        if (ImGui::MenuItem("Select All", "Ctrl+A"))
            textSelect.selectAll();

        ImGui::EndPopup();
    }

    if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 5.0f)
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
    ImGui::End();
}
