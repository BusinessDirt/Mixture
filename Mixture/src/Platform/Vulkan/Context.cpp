#include "mxpch.hpp"
#include "Platform/Vulkan/Context.hpp"

#include "Mixture/Core/Application.hpp"
#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Instance.hpp"
#include "Platform/Vulkan/Surface.hpp"
#include "Platform/Vulkan/PhysicalDevice.hpp"
#include "Platform/Vulkan/Device.hpp"
#include "Platform/Vulkan/Queue.hpp"
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
        if (!s_Instance) throw std::logic_error("No Vulkan context exists");
        return *s_Instance;
    }

    Context::Context(const ApplicationDescription& appDescription, void* windowHandle)
    {
        s_Instance = this;
        try
        {
            m_Instance = CreateRef<Instance>(appDescription);
            m_Surface = CreateScope<Surface>(*m_Instance, windowHandle);
            m_PhysicalDevice = CreateRef<PhysicalDevice>(*m_Instance);
            m_Device = CreateRef<Device>(m_Instance, m_PhysicalDevice);
            m_Swapchain = CreateScope<Swapchain>(*m_PhysicalDevice, *m_Device, *m_Surface, appDescription.Width, appDescription.Height);

            QueueFamilyIndices indices = m_PhysicalDevice->GetQueueFamilies();
            m_GraphicsQueue = CreateScope<Queue>(*m_Device, indices.Graphics, MAX_FRAMES_IN_FLIGHT, "Graphics Queue");
            m_PresentQueue = CreateScope<Queue>(*m_Device, indices.Present, 0, "Present Queue");
            m_ComputeQueue = CreateScope<Queue>(*m_Device, indices.Compute, MAX_FRAMES_IN_FLIGHT, "Compute Queue", indices.Graphics);
            m_TransferQueue = CreateScope<Queue>(*m_Device, indices.Transfer, MAX_FRAMES_IN_FLIGHT, "Transfer Queue", indices.Graphics);

            const uint32_t imageCount = m_Swapchain->GetImageCount();
            m_ImageAvailableSemaphores = CreateScope<Semaphores>(*m_Device, MAX_FRAMES_IN_FLIGHT);
            m_RenderFinishedSemaphores = CreateScope<Semaphores>(*m_Device, imageCount);
            m_TransferFinishedSemaphores = CreateScope<Semaphores>(*m_Device, MAX_FRAMES_IN_FLIGHT);
            m_ComputeFinishedSemaphores = CreateScope<Semaphores>(*m_Device, MAX_FRAMES_IN_FLIGHT);
            m_InFlightFences = CreateScope<Fences>(*m_Device, MAX_FRAMES_IN_FLIGHT, true);

            m_DescriptorLayoutCache = CreateScope<DescriptorLayoutCache>(*m_Device);
            m_DescriptorAllocators = CreateScope<DescriptorAllocators>(*m_Device, MAX_FRAMES_IN_FLIGHT);
            OPAL_INFO("Core/Vulkan", "Vulkan Initialized.");
        }
        catch (...)
        {
            s_Instance = nullptr;
            throw;
        }
    }

    Context::~Context()
    {
        m_Device->WaitForIdle();
        for (auto& cleanup : m_TransferCleanups)
            for (auto& action : cleanup) action();
        for (auto& upload : m_PendingTransferUploads)
            if (upload.Cleanup) upload.Cleanup();
        s_Instance = nullptr;
    }

    DescriptorAllocator* Context::GetCurrentDescriptorAllocator() const { return m_DescriptorAllocators->Get(m_CurrentFrame); }
    DescriptorLayoutCache* Context::GetDescriptorLayoutCache() const { return m_DescriptorLayoutCache.get(); }

    void Context::EnqueueTransferUpload(std::function<void(vk::CommandBuffer)> record, std::function<void()> cleanup)
    {
        if (m_ActiveTransferCommandBuffer)
        {
            record(m_ActiveTransferCommandBuffer);
            m_TransferCleanups[m_CurrentFrame].push_back(std::move(cleanup));
            m_QueueActivity[m_CurrentFrame].Transfer = true;
            return;
        }
        m_PendingTransferUploads.push_back({ std::move(record), std::move(cleanup) });
    }

    void Context::BeginTransferUploads(vk::CommandBuffer commandBuffer)
    {
        m_ActiveTransferCommandBuffer = commandBuffer;
        for (auto& upload : m_PendingTransferUploads)
        {
            upload.Record(commandBuffer);
            m_TransferCleanups[m_CurrentFrame].push_back(std::move(upload.Cleanup));
            m_QueueActivity[m_CurrentFrame].Transfer = true;
        }
        m_PendingTransferUploads.clear();
    }

    void Context::EndTransferUploads()
    {
        m_ActiveTransferCommandBuffer = nullptr;
    }

    uint32_t Context::GetSwapchainWidth() const { return m_Swapchain->GetExtent().width; }
    uint32_t Context::GetSwapchainHeight() const { return m_Swapchain->GetExtent().height; }

    RHI::IGraphicsDevice& Context::GetDevice() const { return *m_Device; }

    void Context::OnResize(uint32_t width, uint32_t height)
    {
        RecreateSwapchain(width, height);
    }

    bool Context::RecreateSwapchain(uint32_t width, uint32_t height)
    {
        if (!m_Swapchain || width == 0 || height == 0) return false;
        m_Device->WaitForIdle();
        m_Swapchain->Recreate(width, height);
        m_RenderFinishedSemaphores = CreateScope<Semaphores>(*m_Device, m_Swapchain->GetImageCount());
        OPAL_LOG_DEBUG("Core/Vulkan", "Swapchain Resized to {} x {}", width, height);
        return true;
    }

    bool Context::RecreateSwapchainFromWindow()
    {
        int width = 0;
        int height = 0;
        Application::Get().GetWindow().GetFramebufferSize(&width, &height);
        if (width <= 0 || height <= 0) return false;
        return RecreateSwapchain(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    }

    RHI::ITexture* Context::BeginFrame()
    {
        if (m_IsFrameStarted)
        {
            OPAL_ERROR("Core/Vulkan", "BeginFrame called but frame already started!");
            return nullptr;
        }

        // Wait for the PREVIOUS frame (using this index) to finish
        if (m_InFlightFences->Wait(m_CurrentFrame) != vk::Result::eSuccess)
        {
            OPAL_ERROR("Core/Vulkan", "Wait for fences failed!");
            return nullptr;
        }
        for (auto& cleanup : m_TransferCleanups[m_CurrentFrame]) cleanup();
        m_TransferCleanups[m_CurrentFrame].clear();

        m_DescriptorAllocators->Get(m_CurrentFrame)->ResetPools();

        uint32_t imageIndex;
        bool acquired = m_Swapchain->AcquireNextImage(&imageIndex, m_ImageAvailableSemaphores->Get(m_CurrentFrame));
        if (!acquired)
        {
            RecreateSwapchainFromWindow();
            return nullptr;
        }

        m_ImageIndex = imageIndex;
        if (m_InFlightFences->Reset(m_CurrentFrame) != vk::Result::eSuccess)
        {
            OPAL_ERROR("Core/Vulkan", "Failed to reset fences!");
            return nullptr;
        }

        m_GraphicsQueue->ResetBuffer(m_CurrentFrame);
        m_TransferQueue->ResetBuffer(m_CurrentFrame);
        m_ComputeQueue->ResetBuffer(m_CurrentFrame);
        m_QueueActivity[m_CurrentFrame] = {};

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

        const auto plan = BuildFrameSubmissionPlan(m_QueueActivity[m_CurrentFrame]);
        if (plan.SubmitTransfer)
            m_TransferQueue->Submit(m_CurrentFrame, { m_TransferFinishedSemaphores->Get(m_CurrentFrame) });
        if (plan.SubmitCompute)
            m_ComputeQueue->Submit(m_CurrentFrame, { m_ComputeFinishedSemaphores->Get(m_CurrentFrame) });

        Vector<vk::Semaphore> waitSemaphores{ m_ImageAvailableSemaphores->Get(m_CurrentFrame) };
        Vector<vk::PipelineStageFlags> waitStages{ vk::PipelineStageFlagBits::eColorAttachmentOutput };
        if (plan.WaitForTransfer)
        {
            waitSemaphores.push_back(m_TransferFinishedSemaphores->Get(m_CurrentFrame));
            waitStages.push_back(vk::PipelineStageFlagBits::eAllCommands);
        }
        if (plan.WaitForCompute)
        {
            waitSemaphores.push_back(m_ComputeFinishedSemaphores->Get(m_CurrentFrame));
            waitStages.push_back(vk::PipelineStageFlagBits::eVertexInput);
        }

        m_GraphicsQueue->Submit(m_CurrentFrame, { m_RenderFinishedSemaphores->Get(m_ImageIndex) },
            std::move(waitSemaphores), std::move(waitStages),
            m_InFlightFences->Get(m_CurrentFrame)
        );

        // Present
        bool success = m_Swapchain->Present(m_ImageIndex, m_RenderFinishedSemaphores->Get(m_ImageIndex), m_PresentQueue->GetHandle());
        if (!success)
            RecreateSwapchainFromWindow();

        // ADVANCE FRAME: 0 -> 1 -> 0 -> 1
        m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        m_IsFrameStarted = false;
    }

    Scope<RHI::ICommandList> Context::GetCommandBuffer()
    {
        return CreateScope<CommandList>(
            FrameCommandContext{
                m_GraphicsQueue->GetBuffer(m_CurrentFrame),
                m_TransferQueue->GetBuffer(m_CurrentFrame),
                m_ComputeQueue->GetBuffer(m_CurrentFrame),
                &m_QueueActivity[m_CurrentFrame],
            }, m_Swapchain->GetImages()[m_ImageIndex]
        );
    }
}
