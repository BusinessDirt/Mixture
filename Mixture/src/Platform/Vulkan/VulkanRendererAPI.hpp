#pragma once

#include "Mixture/Renderer/RendererAPI.hpp"

#include "Platform/Vulkan/VulkanInstance.hpp"
#include "Platform/Vulkan/VulkanDebugMessenger.hpp"

namespace Mixture
{

    class VulkanRendererAPI : public RendererAPI
    {
    public:
        ~VulkanRendererAPI() override;

        void Init(const std::string& applicationName) override;

        void OnWindowResize(uint32_t width, uint32_t height) override;

        bool BeginFrame() override;
        bool EndFrame() override;

    private:
        Scope<VulkanInstance> m_Instance;
        Scope<VulkanDebugMessenger> m_DebugMessenger;
    };
}
