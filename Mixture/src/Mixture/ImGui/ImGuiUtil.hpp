#pragma once

#include "Mixture/Core/Base.hpp"

#include <imgui.h>
#include <imgui_internal.h>

namespace Mixture::ImGuiUtil
{
    void DockSpace(const std::string& name, bool* open, ImGuiDockNodeFlags dockspaceFlags);
    ImVec2 Viewport();
}
