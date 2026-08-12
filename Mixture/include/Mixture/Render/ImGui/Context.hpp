#pragma once

#include "Mixture/Core/Base.hpp"

namespace Mixture::RHI
{
    class IGraphicsContext;
    class ICommandList;
    class ITexture;
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

        /** Registers an RHI texture with ImGui's Vulkan backend and returns an ImTextureID handle. */
        void* GetTextureID(RHI::ITexture* texture) const;

        /** Removes a previously registered ImTextureID handle. */
        void RemoveTextureID(void* textureID) const;

    private:
        struct Impl;
        Scope<Impl> m_Impl;
    };
}
