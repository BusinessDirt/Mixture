#pragma once

#include "Mixture/Renderer/RendererAPI.hpp"

#include "Platform/Vulkan/VulkanInstance.hpp"
#include "Platform/Vulkan/VulkanSurface.hpp"
#include "Platform/Vulkan/VulkanDebugMessenger.hpp"
#include "Platform/Vulkan/VulkanPhysicalDevice.hpp"
#include "Platform/Vulkan/VulkanDevice.hpp"
#include "Platform/Vulkan/VulkanSwapChain.hpp"

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
        Scope<VulkanSurface> m_Surface;
        Scope<VulkanDebugMessenger> m_DebugMessenger;
        Scope<VulkanPhysicalDevice> m_PhysicalDevice;
        Scope<VulkanDevice> m_Device;
        Scope<VulkanSwapChain> m_SwapChain;
    };
}
