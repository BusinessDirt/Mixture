#include "mxpch.hpp"
#include "Platform/Vulkan/Context.hpp"

#include "Mixture/Core/Application.hpp"
#include "Platform/Vulkan/CommandList.hpp"

#include <GLFW/glfw3.h>

namespace Mixture::Vulkan
{

    static Context* s_Instance = nullptr;

    Context& Context::Get()
    {
        return *s_Instance;
    }

    Context::Context(const ApplicationDescription& appDescription, void* windowHandle)
        : RHI::IGraphicsContext(appDescription, windowHandle)
    {
        s_Instance = this;

        m_Instance = CreateRef<Instance>(appDescription);
        m_Surface = CreateRef<Surface>(m_Instance, windowHandle);
        m_PhysicalDevice = CreateRef<PhysicalDevice>(m_Instance->GetHandle());
        m_Device = CreateRef<Device>(m_Instance, m_PhysicalDevice);
        m_Swapchain = CreateRef<Swapchain>(m_PhysicalDevice, m_Device, m_Surface, appDescription.Width, appDescription.Height);

        m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        vk::SemaphoreCreateInfo semaphoreInfo;
        vk::FenceCreateInfo fenceInfo(vk::FenceCreateFlagBits::eSignaled);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            m_ImageAvailableSemaphores[i] = m_Device->GetHandle().createSemaphore(semaphoreInfo);
            m_RenderFinishedSemaphores[i] = m_Device->GetHandle().createSemaphore(semaphoreInfo);
            m_InFlightFences[i] = m_Device->GetHandle().createFence(fenceInfo);
        }

        // Create Command Pool
        QueueFamilyIndices queueFamilyIndices = m_PhysicalDevice->GetQueueFamilies();

        vk::CommandPoolCreateInfo poolInfo;
        poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer; // Allow resetting individual buffers
        poolInfo.queueFamilyIndex = queueFamilyIndices.Graphics.value();

        try
        {
            m_CommandPool = m_Device->GetHandle().createCommandPool(poolInfo);
        }
        catch (vk::SystemError& err)
        {
            OPAL_CRITICAL("Core/Vulkan", "Failed to create Command Pool!");
            exit(-1);
        }

        // Allocate Command Buffers (One per frame in flight)
        m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        vk::CommandBufferAllocateInfo allocInfo;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = (uint32_t) m_CommandBuffers.size();

        m_CommandBuffers = m_Device->GetHandle().allocateCommandBuffers(allocInfo);

        OPAL_INFO("Core/Vulkan", "Vulkan Initialized.");
    }

    Context::~Context()
    {
        m_Device->WaitForIdle();

        m_Device->GetHandle().destroyCommandPool(m_CommandPool);

        // Destroy Synchronization Primitives
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            // Check if the vector was actually populated to avoid crashes on partial init
            if (i < m_ImageAvailableSemaphores.size() && m_ImageAvailableSemaphores[i])
                m_Device->GetHandle().destroySemaphore(m_ImageAvailableSemaphores[i]);

            if (i < m_RenderFinishedSemaphores.size() && m_RenderFinishedSemaphores[i])
                m_Device->GetHandle().destroySemaphore(m_RenderFinishedSemaphores[i]);

            if (i < m_InFlightFences.size() && m_InFlightFences[i])
                m_Device->GetHandle().destroyFence(m_InFlightFences[i]);
        }

        // Clear the vectors to remove the handles
        m_ImageAvailableSemaphores.clear();
        m_RenderFinishedSemaphores.clear();
        m_InFlightFences.clear();

        m_Swapchain.reset();
        m_Device.reset();
        m_PhysicalDevice.reset();
        m_Surface.reset();
        m_Instance.reset();
    }

    void Context::OnResize(uint32_t width, uint32_t height)
    {
        if (m_Swapchain)
        {
            // Handle minimization (width=0) by skipping
            if (width == 0 || height == 0) return;

            m_Swapchain->Recreate(width, height);
            OPAL_INFO("Core/Vulkan", "Swapchain Resized to {} x {}", width, height);
        }
    }

    Ref<RHI::ITexture> Context::BeginFrame()
    {
        auto device = m_Device->GetHandle();

        // Wait for the PREVIOUS frame (using this index) to finish
        if (device.waitForFences(1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX) != vk::Result::eSuccess)
        {
            OPAL_ERROR("Core/Vulkan", "Wait for fences failed!");
            return nullptr;
        }

        uint32_t imageIndex;
        bool acquired = m_Swapchain->AcquireNextImage(imageIndex, m_ImageAvailableSemaphores[m_CurrentFrame]);

        if (!acquired) {
            // TODO: Handle resize...
            return nullptr;
        }

        m_ImageIndex = imageIndex;
        vk::Result result = device.resetFences(1, &m_InFlightFences[m_CurrentFrame]);
        m_CommandBuffers[m_CurrentFrame].reset();

        return m_Swapchain->GetTexture(imageIndex);
    }

    void Context::EndFrame()
    {
        vk::SubmitInfo submitInfo;

        vk::Semaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
        vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame];

        vk::Semaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        // Signal the fence for THIS frame
        m_Device->GetGraphicsQueue().submit(submitInfo, m_InFlightFences[m_CurrentFrame]);

        // Present
        bool success = m_Swapchain->Present(m_ImageIndex, m_RenderFinishedSemaphores[m_CurrentFrame]);
        if (!success)
        {
            // TODO: Handle resize
        }

        // ADVANCE FRAME: 0 -> 1 -> 0 -> 1
        m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    Ref<RHI::ICommandList> Context::GetCommandBuffer()
    {
        return CreateRef<CommandList>(m_CommandBuffers[m_CurrentFrame]);
    }
}
