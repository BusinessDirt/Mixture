#pragma once

/**
 * @file Mixture.hpp
 * @brief Main header file for the Mixture engine.
 * 
 * Includes core components like Application, Layer, and LayerStack.
 */

#include "Mixture/Render/Graph/RenderGraph.hpp"
#include "Mixture/Render/RHI/RHI.hpp"
#include "Mixture/Render/ImGui/ImGuiPass.hpp"
#include "Mixture/Render/ImGui/Theme.hpp"
#include "Mixture/Render/ImGui/ThemeManager.hpp"
#include "Mixture/Render/RenderStats.hpp"

#include "Mixture/Core/Base.hpp"
#include "Mixture/Core/Application.hpp"
#include "Mixture/Core/Layer.hpp"
#include "Mixture/Core/LayerStack.hpp"

#include "Mixture/Scene/Components.hpp"
#include "Mixture/Scene/Entity.hpp"
#include "Mixture/Scene/Scene.hpp"

#include <imgui.h>
