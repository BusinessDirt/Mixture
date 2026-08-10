#pragma once

#include "Mixture/Core/Base.hpp"

namespace Mixture::RHI
{
    class IGraphicsContext;
    class ICommandList;
}

namespace Mixture
{
    /** Application-owned Dear ImGui lifecycle and renderer integration. */
    class ImGuiContext
    {
    public:
        OPAL_NON_COPIABLE(ImGuiContext);

        ImGuiContext(void* windowHandle, RHI::IGraphicsContext& graphicsContext);
        ~ImGuiContext();

        void BeginFrame();
        void EndFrame();
        void Render(RHI::ICommandList* commandList) const;

    private:
        struct Impl;
        Scope<Impl> m_Impl;
    };
}
