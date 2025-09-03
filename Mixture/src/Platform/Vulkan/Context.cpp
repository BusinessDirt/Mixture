#include "mxpch.hpp"
#include "Platform/Vulkan/Context.hpp"

#include <Opal/Base.hpp>

namespace Mixture::Vulkan
{
    Scope<Instance> Context::Instance = nullptr;
    Scope<DebugMessenger> Context::DebugMessenger = nullptr;
    Scope<WindowSurface> Context::WindowSurface = nullptr;
    Scope<PhysicalDevice> Context::PhysicalDevice = nullptr;
    Scope<Device> Context::Device = nullptr;
    Scope<Swapchain> Context::Swapchain = nullptr;
    Scope<CommandPool> Context::CommandPool = nullptr;
    Scope<CommandBuffers> Context::CommandBuffers = nullptr;
    Scope<DescriptorPool> Context::DescriptorPool = nullptr;
    
    uint32_t Context::CurrentImageIndex = 0;
    VkCommandBuffer Context::CurrentCommandBuffer = VK_NULL_HANDLE;

    Scope<Renderpass> Context::ImGuiRenderpass = nullptr;
    Vector<Scope<FrameBuffer>> Context::ImGuiFrameBuffers;
}
