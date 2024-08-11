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
#include "Platform/Vulkan/Command/CommandPool.hpp"
#include "Platform/Vulkan/Pipeline/GraphicsPipeline.hpp"

namespace Mixture::Vulkan
{
    struct Context
    {
        Context(const Context&) = delete;
        Context& operator=(const Context&) = delete;

        static Context& Get();

        // Public members
        Scope<Instance> Instance;
        Scope<Surface> Surface;
        Scope<DebugMessenger> DebugMessenger;
        Scope<PhysicalDevice> PhysicalDevice;
        Scope<Device> Device;
        Scope<SwapChain> SwapChain;
        Scope<CommandPool> CommandPool;

    private:
        Context() = default;
        static Context* s_Instance;
        static std::mutex s_Mutex;
    };
}
