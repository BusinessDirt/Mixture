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
    class Context
    {
    public:
        Context(const Context&) = delete;
        Context& operator=(const Context&) = delete;

        static Context& Get();

        Context() = default;
        ~Context() = default;

        void Initialize(const std::string& applicationName);
        void Shutdown();

        void RebuildSwapChain();
        void CreateImGuiViewport();
        VkResult AcquireSwapChainImage() { return m_SwapChain->AcquireNextImage(); }
        VkResult SubmitSwapChainCommandBuffers(const std::vector<CommandBuffer>& buffers) { return m_SwapChain->SubmitCommandBuffers(buffers); }

    public:
#define CONTEXT_MEMBER(className, varName) private: Scope<className> m_##varName = nullptr; public: const className& Get##varName() const { return *m_##varName; }
        CONTEXT_MEMBER(Instance, Instance)
        CONTEXT_MEMBER(Surface, Surface)
        CONTEXT_MEMBER(DebugMessenger, DebugMessenger)
        CONTEXT_MEMBER(PhysicalDevice, PhysicalDevice)
        CONTEXT_MEMBER(Device, Device)
        CONTEXT_MEMBER(SwapChain, SwapChain)
        CONTEXT_MEMBER(CommandPool, CommandPool)
        CONTEXT_MEMBER(DescriptorSetManager, GlobalDescriptorSet)
        CONTEXT_MEMBER(DescriptorSetManager, InstanceDescriptorSet)
        CONTEXT_MEMBER(Viewport, ImGuiViewport)

        uint32_t* GetCurrentImageIndexPtr() { return &m_CurrentImageIndex; }
        uint32_t GetCurrentImageIndex() { return m_CurrentImageIndex; }

    private:
        uint32_t m_CurrentImageIndex = 0;

    private:
        static Scope<Context> s_Instance;
        static std::mutex s_Mutex;
    };
}
