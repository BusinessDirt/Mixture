#pragma once

#include "Platform/Vulkan/Base.hpp"
#include "Platform/Vulkan/Instance.hpp"
#include "Platform/Vulkan/DebugMessenger.hpp"
#include "Platform/Vulkan/WindowSurface.hpp"
#include "Platform/Vulkan/PhysicalDevice.hpp"
#include "Platform/Vulkan/Device.hpp"
#include "Platform/Vulkan/Swapchain.hpp"
#include "Platform/Vulkan/Descriptor/Pool.hpp"
#include "Platform/Vulkan/Command/Pool.hpp"
#include "Platform/Vulkan/Command/Buffers.hpp"

namespace Mixture::Vulkan
{
    struct Context
    {
        static Scope<Instance> Instance;
        static Scope<DebugMessenger> DebugMessenger;
        static Scope<WindowSurface> WindowSurface;
        static Scope<PhysicalDevice> PhysicalDevice;
        static Scope<Device> Device;
        static Scope<Swapchain> Swapchain;
        static Scope<CommandPool> CommandPool;
        static Scope<CommandBuffers> CommandBuffers;
        static Scope<DescriptorPool> DescriptorPool;
        
        static uint32_t CurrentImageIndex;
        static VkCommandBuffer CurrentCommandBuffer;

        static Scope<Renderpass> ImGuiRenderpass;
        static Vector<Scope<FrameBuffer>> ImGuiFrameBuffers;

        static void WaitForDevice() { vkDeviceWaitIdle(Device->GetHandle()); }
    };
}
