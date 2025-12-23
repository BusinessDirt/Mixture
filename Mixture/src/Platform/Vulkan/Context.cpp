#include "mxpch.hpp"
#include "Platform/Vulkan/Context.hpp"

#include "Mixture/Core/Application.hpp"

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Instance.hpp"
#include "Platform/Vulkan/Surface.hpp"
#include "Platform/Vulkan/PhysicalDevice.hpp"
#include "Platform/Vulkan/Device.hpp"
#include "Platform/Vulkan/Swapchain.hpp"

#include "Platform/Vulkan/Command/Buffers.hpp"
#include "Platform/Vulkan/Command/List.hpp"
#include "Platform/Vulkan/Command/Pool.hpp"

#include "Platform/Vulkan/Sync/Fences.hpp"
#include "Platform/Vulkan/Sync/Semaphores.hpp"

#include "Platform/Vulkan/Descriptors/LayoutCache.hpp"
#include "Platform/Vulkan/Descriptors/Allocator.hpp"

#include <GLFW/glfw3.h>

namespace Mixture::Vulkan
{

    static Context* s_Instance = nullptr;
    static const int MAX_FRAMES_IN_FLIGHT = 2;

    Context& Context::Get()
    {
        return *s_Instance;
    }

    Context::Context(const ApplicationDescription& appDescription, void* windowHandle)
    {
        s_Instance = this;

        m_Instance = CreateScope<Instance>(appDescription);
        m_Surface = CreateScope<Surface>(*m_Instance, windowHandle);
        m_PhysicalDevice = CreateScope<PhysicalDevice>(*m_Instance);
        m_Device = CreateScope<Device>(*m_Instance, *m_PhysicalDevice);
        m_Swapchain = CreateScope<Swapchain>(*m_PhysicalDevice, *m_Device, *m_Surface, appDescription.Width, appDescription.Height);

        size_t imageCount = m_Swapchain->GetImageCount();
        m_ImageAvailableSemaphores = CreateScope<Semaphores>(*m_Device, MAX_FRAMES_IN_FLIGHT);
        m_RenderFinishedSemaphores = CreateScope<Semaphores>(*m_Device, imageCount);
        m_InFlightFences = CreateScope<Fences>(*m_Device, MAX_FRAMES_IN_FLIGHT, true);

        // Create Command Pool
        QueueFamilyIndices queueFamilyIndices = m_PhysicalDevice->GetQueueFamilies();
        m_CommandPool = CreateScope<CommandPool>(*m_Device, queueFamilyIndices);
        m_CommandBuffers = CreateScope<CommandBuffers>(*m_Device, *m_CommandPool, MAX_FRAMES_IN_FLIGHT);

        m_DescriptorLayoutCache = CreateScope<DescriptorLayoutCache>(*m_Device);
        m_DescriptorAllocators = CreateScope<DescriptorAllocators>(*m_Device, MAX_FRAMES_IN_FLIGHT);

        OPAL_INFO("Core/Vulkan", "Vulkan Initialized.");
    }

    Context::~Context()
    {
        m_Device->WaitForIdle();
        s_Instance = nullptr;
    }

    DescriptorAllocator* Context::GetCurrentDescriptorAllocator() const { return m_DescriptorAllocators->Get(m_CurrentFrame); }
    DescriptorLayoutCache* Context::GetDescriptorLayoutCache() const { return m_DescriptorLayoutCache.get(); }

    uint32_t Context::GetSwapchainWidth() { return m_Swapchain->GetExtent().width; }
    uint32_t Context::GetSwapchainHeight() { return m_Swapchain->GetExtent().height; }

    RHI::IGraphicsDevice& Context::GetDevice() const { return *m_Device; }

    void Context::OnResize(uint32_t width, uint32_t height)
    {
        if (m_Swapchain)
        {
            // Handle minimization (width=0) by skipping
            if (width == 0 || height == 0) return;

            m_Device->WaitForIdle();

            m_Swapchain->Recreate(width, height);

            // Recreate Semaphores (Image count might have changed!)
            size_t imageCount = m_Swapchain->GetImageCount();
            m_RenderFinishedSemaphores.reset();
            m_RenderFinishedSemaphores = CreateScope<Semaphores>(*m_Device, imageCount);

            OPAL_INFO("Core/Vulkan", "Swapchain Resized to {} x {}", width, height);
        }
    }

    Ref<RHI::ITexture> Context::BeginFrame()
    {
        if (m_IsFrameStarted)
        {
            OPAL_ERROR("Core/Vulkan", "BeginFrame called but frame already started!");
            return nullptr;
        }

        auto device = m_Device->GetHandle();

        // Wait for the PREVIOUS frame (using this index) to finish
        if (m_InFlightFences->Wait(m_CurrentFrame) != vk::Result::eSuccess)
        {
            OPAL_ERROR("Core/Vulkan", "Wait for fences failed!");
            return nullptr;
        }

        m_DescriptorAllocators->Get(m_CurrentFrame)->ResetPools();

        uint32_t imageIndex;
        bool acquired = m_Swapchain->AcquireNextImage(imageIndex, m_ImageAvailableSemaphores->Get(m_CurrentFrame));

        if (!acquired) {
            // TODO: Handle resize...
            return nullptr;
        }

        m_ImageIndex = imageIndex;
        if (m_InFlightFences->Reset(m_CurrentFrame) != vk::Result::eSuccess)
        {
            OPAL_ERROR("Core/Vulkan", "Failed to reset fences!");
            return nullptr;
        }

        m_CommandBuffers->Reset(m_CurrentFrame);

        m_IsFrameStarted = true;

        return m_Swapchain->GetTexture(imageIndex);
    }

    void Context::EndFrame()
    {
        if (!m_IsFrameStarted)
        {
            OPAL_ERROR("Core/Vulkan", "EndFrame called but frame not started!");
            return;
        }

        vk::Semaphore signalSemaphores[] = { m_RenderFinishedSemaphores->Get(m_ImageIndex) };
        vk::Semaphore waitSemaphores[] = { m_ImageAvailableSemaphores->Get(m_CurrentFrame) };
        vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

        vk::SubmitInfo submitInfo;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = m_CommandBuffers->GetPointer(m_CurrentFrame);
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        // Signal the fence for THIS frame
        m_Device->GetGraphicsQueue().submit(submitInfo, m_InFlightFences->Get(m_CurrentFrame));

        // Present
        bool success = m_Swapchain->Present(m_ImageIndex, m_RenderFinishedSemaphores->Get(m_ImageIndex));
        if (!success)
        {
            // TODO: Handle resize
        }

        // ADVANCE FRAME: 0 -> 1 -> 0 -> 1
        m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        m_IsFrameStarted = false;
    }

    Ref<RHI::ICommandList> Context::GetCommandBuffer()
    {
        return CreateRef<CommandList>(m_CommandBuffers->Get(m_CurrentFrame), m_Swapchain->GetImages()[m_ImageIndex]);
    }
}
