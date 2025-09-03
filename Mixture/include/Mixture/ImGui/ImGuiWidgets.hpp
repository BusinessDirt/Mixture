#pragma once

#include <imgui.h>
#include <string_view>

namespace Mixture::ImGuiWidgets
{
    ImGuiID Dockspace(const std::string_view& windowTitle = "Dockspace", bool fullscreen = true, ImGuiWindowFlags windowFlagsOverride = 0, ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None, bool* pOpen = nullptr);
}

