#include "mxpch.hpp"
#include "Platform/Vulkan/RendererAPI.hpp"

#include "Mixture/Core/Application.hpp"

#ifdef OPAL_PLATFORM_DARWIN
    #include <vulkan/vulkan_metal.h>
#endif

#ifdef OPAL_PLATFORM_WINDOWS
    #include <vulkan/vulkan_win32.h>
#endif

#include <backends/imgui_impl_glfw.cpp>
#include <backends/imgui_impl_vulkan.cpp>

namespace Mixture::Vulkan
{
    namespace Util
    {
        namespace {
            std::vector<const char*> GetRequiredLayers()
            {
                std::vector<const char*> requiredLayers;

                // Common layers for validation
#ifndef OPAL_DIST
                requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

                return requiredLayers;
            }

            std::vector<const char*> GetRequiredInstanceExtensions()
            {
                std::vector<const char*> requiredExtensions;

                // Common extensions
                requiredExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
            
#if defined(OPAL_PLATFORM_WINDOWS)
                requiredExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(OPAL_PLATFORM_DARWIN)
                requiredExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
                requiredExtensions.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
#elif defined(OPAL_PLATFORM_LINUX)
                requiredExtensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#endif

#ifndef OPAL_DIST
                requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

                return requiredExtensions;
            }

            std::vector<const char*> GetRequiredDeviceExtensions()
            {
                std::vector<const char*> requiredExtensions;
            
                // Swapchain
                requiredExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
            
#ifdef OPAL_PLATFORM_DARWIN
                // Required for MoltenVK
                requiredExtensions.push_back("VK_KHR_portability_subset");
#endif

                return requiredExtensions;
            }
        }
    }
    
    void RendererAPI::Initialize(const std::string& applicationName)
    {
        Context::Instance = CreateScope<Instance>(applicationName, Util::GetRequiredLayers(), Util::GetRequiredInstanceExtensions());
        
#ifndef OPAL_DIST
        Context::DebugMessenger = CreateScope<DebugMessenger>();
#endif
        
        Context::WindowSurface = CreateScope<WindowSurface>();
        Context::PhysicalDevice = CreateScope<PhysicalDevice>(Util::GetRequiredDeviceExtensions());
        Context::Device = CreateScope<Device>(Util::GetRequiredLayers(), Util::GetRequiredDeviceExtensions());
        Context::CommandPool = CreateScope<CommandPool>();
        Context::Swapchain = CreateScope<Swapchain>();
        Context::CommandBuffers = CreateScope<CommandBuffers>(Context::CommandPool->GetHandle(), Swapchain::MAX_FRAMES_IN_FLIGHT);
        
        Context::DescriptorPool = CreateScope<DescriptorPool>();

        {
            const Swapchain& swapchain = *Context::Swapchain;
        
            const VkFormat format = swapchain.GetImageFormat();
            Context::ImGuiRenderpass = CreateScope<Renderpass>(format, false,
                VK_ATTACHMENT_LOAD_OP_LOAD, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        
            CreateImGuiFrameBuffers();

            // Init ImGui Vulkan backend with the VkRenderPass handle
            ImGui_ImplVulkan_InitInfo initInfo{};
            initInfo.ApiVersion = VK_API_VERSION_1_2;
            initInfo.Instance = Context::Instance->GetHandle();
            initInfo.PhysicalDevice = Context::PhysicalDevice->GetHandle();
            initInfo.Device = Context::Device->GetHandle();
            initInfo.QueueFamily = Context::PhysicalDevice->GetQueueFamilyIndices().Graphics.value();
            initInfo.Queue = Context::Device->GetGraphicsQueue();
            initInfo.PipelineCache = VK_NULL_HANDLE;
            initInfo.DescriptorPool = Context::DescriptorPool->GetGlobalHandle();
            initInfo.RenderPass = Context::ImGuiRenderpass->GetHandle();
            initInfo.Subpass = 0;
            initInfo.MinImageCount = Swapchain::MAX_FRAMES_IN_FLIGHT;
            initInfo.ImageCount = static_cast<uint32_t>(swapchain.GetImageCount());
            initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
            initInfo.Allocator = nullptr;
            initInfo.CheckVkResultFn = [](const VkResult res) { OPAL_CORE_ASSERT(res == VK_SUCCESS, "ImGui Vulkan backend error") };

            ImGui_ImplVulkan_Init(&initInfo);
            ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()), true);
        }
    }

    void RendererAPI::DestroyImGuiContext()
    {
        if (!Context::ImGuiRenderpass) return;

        Context::WaitForDevice();
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
    }

    void RendererAPI::Shutdown()
    {
        Context::ImGuiFrameBuffers.clear();
        Context::ImGuiRenderpass = nullptr;
        
        Context::DescriptorPool = nullptr;
        Context::CommandBuffers = nullptr;
        Context::Swapchain = nullptr;
        Context::CommandPool = nullptr;
        Context::Device = nullptr;
        Context::PhysicalDevice = nullptr;
        Context::WindowSurface = nullptr;
        Context::DebugMessenger = nullptr;
        Context::Instance = nullptr;
    }

    void RendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        RebuildSwapchain();
    }

    void RendererAPI::SetClearColor(const glm::vec4& color)
    {
        m_ClearColor = { { color.r, color.g, color.b, color.a } };
    }

    void RendererAPI::BeginFrame()
    {
        OPAL_CORE_ASSERT(!m_IsFrameStarted, "Can't call BeginFrame() while already in progess!")

        const VkResult result = Context::Swapchain->AcquireNextImage();
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RebuildSwapchain();
            Context::CurrentCommandBuffer = VK_NULL_HANDLE;
            return;
        }

        OPAL_CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire Swapchain image!")

        m_IsFrameStarted = true;

        const VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo), "Failed to begin recording command buffer!")

        Context::CurrentCommandBuffer = commandBuffer;
    }

    void RendererAPI::EndFrame()
    {
        OPAL_CORE_ASSERT(m_IsFrameStarted, "Can't call EndFrame() while frame is not in progress!")
        VK_ASSERT(vkEndCommandBuffer(Context::CurrentCommandBuffer), "Failed to record command buffer!")

        const std::vector commandBuffers = { Context::CurrentCommandBuffer };
        if (const VkResult result = Context::Swapchain->SubmitCommandBuffers(commandBuffers);
            result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            RebuildSwapchain();
        }
        else
        {
            OPAL_CORE_ASSERT(result == VK_SUCCESS, "Failed to present Swapchain image!")
        }

        m_IsFrameStarted = false;
        Context::CurrentCommandBuffer = VK_NULL_HANDLE;
        Context::WaitForDevice();
    }

    void RendererAPI::BeginSceneRenderpass()
    {
        OPAL_CORE_ASSERT(m_IsFrameStarted, "Can't call BeginRenderPass() if frame is not in progress!")

        VkRenderPassBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.renderPass = Context::Swapchain->GetRenderpass().GetHandle();
        beginInfo.framebuffer = Context::Swapchain->GetFramebuffer(Context::CurrentImageIndex).GetHandle();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = m_ClearColor;
        clearValues[1].depthStencil = {.depth = 1.0f, .stencil = 0 };

        beginInfo.renderArea.offset = {.x = 0, .y = 0 };
        beginInfo.renderArea.extent = Context::Swapchain->GetExtent();
        beginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        beginInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(Context::CurrentCommandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(Context::Swapchain->GetWidth());
        viewport.height = static_cast<float>(Context::Swapchain->GetHeight());
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        const VkRect2D scissor{ { 0, 0 }, Context::Swapchain->GetExtent() };

        vkCmdSetViewport(Context::CurrentCommandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(Context::CurrentCommandBuffer, 0, 1, &scissor);
    }

    void RendererAPI::EndSceneRenderpass()
    {
        OPAL_CORE_ASSERT(m_IsFrameStarted, "Can't call EndRenderPass() if frame is not in progress!")
        OPAL_CORE_ASSERT(Context::CurrentCommandBuffer == GetCurrentCommandBuffer(), "Can't end render pass on command buffer from a different frame!")

        vkCmdEndRenderPass(Context::CurrentCommandBuffer);
    }

    void RendererAPI::BeginImGuiImpl()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
    }

    void RendererAPI::RenderImGui()
    {
        VkRenderPassBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.renderPass = Context::ImGuiRenderpass->GetHandle();
        beginInfo.framebuffer = Context::ImGuiFrameBuffers[Context::CurrentImageIndex]->GetHandle();
        beginInfo.renderArea.offset = {.x = 0, .y = 0};
        beginInfo.renderArea.extent.width = Context::Swapchain->GetWidth();
        beginInfo.renderArea.extent.height = Context::Swapchain->GetHeight();
        beginInfo.clearValueCount = 0;
        beginInfo.pClearValues = nullptr;

        vkCmdBeginRenderPass(Context::CurrentCommandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
        
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), Context::CurrentCommandBuffer);

        vkCmdEndRenderPass(Context::CurrentCommandBuffer);
        if (const ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    void RendererAPI::RebuildSwapchain()
    {
        Context::WaitForDevice();

        std::shared_ptr oldSwapchain = std::move(Context::Swapchain);
        Context::Swapchain = CreateScope<Swapchain>(oldSwapchain);

        OPAL_CORE_ASSERT(oldSwapchain->CompareSwapFormats(*Context::Swapchain.get()), "Swap chain image (or depth) format has changed!")
        
        Context::ImGuiFrameBuffers.clear();
        CreateImGuiFrameBuffers();
    }

    void RendererAPI::CreateImGuiFrameBuffers()
    {
        const Swapchain& swapchain = *Context::Swapchain;
        Context::ImGuiFrameBuffers.resize(swapchain.GetImageCount());
        for (uint32_t i = 0; i < swapchain.GetImageCount(); i++)
        {
            const FrameBuffer& fb = swapchain.GetFramebuffer(i);
            Context::ImGuiFrameBuffers[i] = CreateScope<FrameBuffer>(VK_NULL_HANDLE, fb.GetImage(), swapchain.GetExtent(),
                fb.GetFormat(), Context::ImGuiRenderpass->GetHandle());
        }
    }
}
