#pragma once

#include "Mixture/Core/Base.hpp"

#include <mutex>

#include "Platform/Vulkan/VulkanInstance.hpp"
#include "Platform/Vulkan/VulkanSurface.hpp"
#include "Platform/Vulkan/VulkanDebugMessenger.hpp"
#include "Platform/Vulkan/VulkanPhysicalDevice.hpp"
#include "Platform/Vulkan/VulkanDevice.hpp"
#include "Platform/Vulkan/VulkanSwapChain.hpp"
#include "Platform/Vulkan/Pipeline/VulkanGraphicsPipeline.hpp"

namespace Mixture
{
    struct VulkanContext
    {
        VulkanContext(const VulkanContext&) = delete;
        VulkanContext& operator=(const VulkanContext&) = delete;

        static VulkanContext& Get();

        // Public members
        Scope<VulkanInstance> Instance;
        Scope<VulkanSurface> Surface;
        Scope<VulkanDebugMessenger> DebugMessenger;
        Scope<VulkanPhysicalDevice> PhysicalDevice;
        Scope<VulkanDevice> Device;
        Scope<VulkanSwapChain> SwapChain;
        Scope<VulkanGraphicsPipeline> GraphicsPipeline;

    private:
        VulkanContext() = default;
        static VulkanContext* s_Instance;
        static std::mutex s_Mutex;
    };
}
