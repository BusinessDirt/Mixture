#pragma once
#include "Mixture/Core/Base.hpp"

namespace Mixture
{
    class ImGuiContext
    {
    public:
        static void Initialize();
        static void Shutdown();

        static void BeginFrame();
        static void EndFrame();
    };
}
