#pragma once

#include "Mixture/Core/Base.hpp"

#include <imgui.h>

namespace Mixture::ImGuiUtil
{
    void DockSpace(const std::string& name, bool* open, ImGuiDockNodeFlags dockspaceFlags);
}
