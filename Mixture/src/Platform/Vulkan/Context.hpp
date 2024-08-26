#pragma once

#include "Mixture/Core/Base.hpp"

#include <mutex>

#include "Platform/Vulkan/Instance.hpp"
#include "Platform/Vulkan/Surface.hpp"
#include "Platform/Vulkan/DebugMessenger.hpp"
#include "Platform/Vulkan/PhysicalDevice.hpp"
#include "Platform/Vulkan/Device.hpp"
#include "Platform/Vulkan/SwapChain.hpp"
#include "Platform/Vulkan/RenderPass.hpp"
#include "Platform/Vulkan/Viewport.hpp"
#include "Platform/Vulkan/Command/CommandPool.hpp"
#include "Platform/Vulkan/Pipeline/GraphicsPipeline.hpp"
#include "Platform/Vulkan/Descriptor/DescriptorSetManager.hpp"

namespace Mixture::Vulkan
{
    struct Context
    {
        Context(const Context&) = delete;
        Context& operator=(const Context&) = delete;

        static Context& Get();

        // Public members
        Scope<Instance> Instance = nullptr;
        Scope<Surface> Surface = nullptr;
        Scope<DebugMessenger> DebugMessenger = nullptr;
        Scope<PhysicalDevice> PhysicalDevice = nullptr;
        Scope<Device> Device = nullptr;
        Scope<SwapChain> SwapChain = nullptr;
        Scope<CommandPool> CommandPool = nullptr;

        Scope<DescriptorSetManager> GlobalDescriptors = nullptr;
        Scope<DescriptorSetManager> InstanceDescriptors = nullptr;

        Scope<Viewport> ImGuiViewport = nullptr;
        uint32_t CurrentImageIndex = 0;

    private:
        Context() = default;
        static Context* s_Instance;
        static std::mutex s_Mutex;
    };
}
