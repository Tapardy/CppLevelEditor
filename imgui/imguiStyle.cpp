#include "imgui.h"
#include "./imguiStyle.h"

void SetCustomImGuiStyle()
{
    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 *colors = style.Colors;

    // caching here is pretty easy so i can add more colors and reuse them more often
    ImVec4 text = ImVec4(205 / 255.0f, 214 / 255.0f, 244 / 255.0f, 1.0f);     // #cdd6f4
    ImVec4 overlay1 = ImVec4(127 / 255.0f, 132 / 255.0f, 156 / 255.0f, 1.0f); // #7f849c
    ImVec4 base = ImVec4(30 / 255.0f, 30 / 255.0f, 46 / 255.0f, 1.0f);        // #1e1e2e
    ImVec4 mantle = ImVec4(24 / 255.0f, 24 / 255.0f, 37 / 255.0f, 1.0f);      // #181825
    ImVec4 crust = ImVec4(17 / 255.0f, 17 / 255.0f, 27 / 255.0f, 1.0f);       // #11111b
    ImVec4 surface0 = ImVec4(49 / 255.0f, 50 / 255.0f, 68 / 255.0f, 1.0f);    // #313244
    ImVec4 surface1 = ImVec4(69 / 255.0f, 71 / 255.0f, 90 / 255.0f, 1.0f);    // #45475a
    ImVec4 surface2 = ImVec4(88 / 255.0f, 91 / 255.0f, 112 / 255.0f, 1.0f);   // #585b70
    ImVec4 mauve = ImVec4(203 / 255.0f, 166 / 255.0f, 247 / 255.0f, 1.0f);    // #cba6f7
    ImVec4 lavender = ImVec4(180 / 255.0f, 190 / 255.0f, 254 / 255.0f, 1.0f); // #b4befe
    ImVec4 blue = ImVec4(137 / 255.0f, 180 / 255.0f, 250 / 255.0f, 1.0f);     // #89b4fa
    ImVec4 sapphire = ImVec4(116 / 255.0f, 199 / 255.0f, 236 / 255.0f, 1.0f); // #74c7ec
    ImVec4 sky = ImVec4(137 / 255.0f, 220 / 255.0f, 235 / 255.0f, 1.0f);      // #89dceb

    colors[ImGuiCol_Text] = text;
    colors[ImGuiCol_TextDisabled] = overlay1;
    colors[ImGuiCol_WindowBg] = base;
    colors[ImGuiCol_ChildBg] = mantle;
    colors[ImGuiCol_PopupBg] = surface1;
    colors[ImGuiCol_Border] = lavender;
    colors[ImGuiCol_BorderShadow] = crust;
    colors[ImGuiCol_FrameBg] = surface0;
    colors[ImGuiCol_FrameBgHovered] = surface2;
    colors[ImGuiCol_FrameBgActive] = surface1;
    colors[ImGuiCol_TitleBg] = surface1;
    colors[ImGuiCol_TitleBgActive] = surface2;
    colors[ImGuiCol_TitleBgCollapsed] = crust;
    colors[ImGuiCol_MenuBarBg] = surface1;
    colors[ImGuiCol_ScrollbarBg] = surface0;
    colors[ImGuiCol_ScrollbarGrab] = surface2;
    colors[ImGuiCol_ScrollbarGrabHovered] = lavender;
    colors[ImGuiCol_ScrollbarGrabActive] = mauve;
    colors[ImGuiCol_CheckMark] = mauve;
    colors[ImGuiCol_SliderGrab] = mauve;
    colors[ImGuiCol_SliderGrabActive] = lavender;
    colors[ImGuiCol_Button] = ImVec4(mauve.x, mauve.y, mauve.z, 0.3f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(mauve.x, mauve.y, mauve.z, 0.7f);
    colors[ImGuiCol_ButtonActive] = mauve;
    colors[ImGuiCol_Header] = ImVec4(mauve.x, mauve.y, mauve.z, 0.2f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(mauve.x, mauve.y, mauve.z, 0.5f);
    colors[ImGuiCol_HeaderActive] = ImVec4(mauve.x, mauve.y, mauve.z, 0.7f);
    colors[ImGuiCol_Separator] = lavender;
    colors[ImGuiCol_SeparatorHovered] = mauve;
    colors[ImGuiCol_SeparatorActive] = mauve;
    colors[ImGuiCol_ResizeGrip] = surface2;
    colors[ImGuiCol_ResizeGripHovered] = lavender;
    colors[ImGuiCol_ResizeGripActive] = mauve;

    colors[ImGuiCol_Tab] = ImVec4(mauve.x * 0.85f, mauve.y * 0.85f, mauve.z, 1.0f);
    colors[ImGuiCol_TabHovered] = ImVec4(mauve.x, mauve.y, mauve.z, 1.0f);
    colors[ImGuiCol_TabActive] = ImVec4(mauve.x * 0.7f, mauve.y * 0.7f, mauve.z, 1.0f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(surface1.x, surface1.y, surface1.z, 1.0f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(mauve.x * 0.5f, mauve.y * 0.5f, mauve.z, 1.0f);

    colors[ImGuiCol_DockingPreview] = sapphire;
    colors[ImGuiCol_DockingEmptyBg] = base;
    colors[ImGuiCol_PlotLines] = blue;
    colors[ImGuiCol_PlotLinesHovered] = sky;
    colors[ImGuiCol_PlotHistogram] = sapphire;
    colors[ImGuiCol_PlotHistogramHovered] = lavender;
    colors[ImGuiCol_TableHeaderBg] = surface1;
    colors[ImGuiCol_TableBorderStrong] = surface2;
    colors[ImGuiCol_TableBorderLight] = surface1;
    colors[ImGuiCol_TableRowBg] = mantle;
    colors[ImGuiCol_TableRowBgAlt] = base;
    colors[ImGuiCol_TextSelectedBg] = ImVec4(mauve.x, mauve.y, mauve.z, 0.3f);
    colors[ImGuiCol_DragDropTarget] = sapphire;
    colors[ImGuiCol_NavHighlight] = lavender;
    colors[ImGuiCol_NavWindowingHighlight] = lavender;
    colors[ImGuiCol_NavWindowingDimBg] = crust;
    colors[ImGuiCol_ModalWindowDimBg] = crust;

    style.WindowRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 3.0f;
    style.ChildRounding = 4.0f;

    style.WindowTitleAlign = ImVec2(0.50f, 0.50f);
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.FramePadding = ImVec2(5.0f, 4.0f);
    style.ItemSpacing = ImVec2(6.0f, 6.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
    style.IndentSpacing = 22.0f;

    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 10.0f;

    style.AntiAliasedLines = true;
    style.AntiAliasedFill = true;
}
