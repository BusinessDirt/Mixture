#include "mxpch.hpp"
#include "ImGuiRenderer.hpp"

#include "Mixture/Core/Application.hpp"
#include "Mixture/ImGui/ImGuiTheme.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Command/SingleTimeCommand.hpp"
#include "Platform/Vulkan/Image/Sampler.hpp"

#include <imgui_internal.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

// Emedded font
#include "Mixture/ImGui/Roboto-Regular.embed"
#include "Mixture/ImGui/Roboto-Bold.embed"
#include "Mixture/ImGui/Roboto-Italic.embed"

namespace Mixture::Vulkan
{
    ImGuiRenderer::ImGuiRenderer() {}

    ImGuiRenderer::~ImGuiRenderer()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        
        for (auto& frameBuffer : m_FrameBuffers) frameBuffer = nullptr;
        m_RenderPass = nullptr;
        m_DescriptorPool = nullptr;
    }

    void ImGuiRenderer::Init()
    {
        const SwapChain& swapChain = *Context::Get().SwapChain;
        
        m_DescriptorPool = DescriptorPool::CreateImGuiDescriptorPool();
        m_RenderPass = CreateScope<RenderPass>(swapChain.GetImageFormat(), false, VK_ATTACHMENT_LOAD_OP_LOAD, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        
        m_FrameBuffers.resize(swapChain.GetImageCount());
        for (int i = 0; i < swapChain.GetImageCount(); i++)
        {
            m_FrameBuffers[i] = CreateScope<FrameBuffer>(nullptr, swapChain.GetFrameBuffer(i).GetImage(), swapChain.GetExtent(), swapChain.GetImageFormat(), m_RenderPass->GetHandle());
        }
        
        
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // Theme colors
        SetMixtureTheme();

        // Style
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowPadding = ImVec2(10.0f, 10.0f);
        style.FramePadding = ImVec2(8.0f, 6.0f);
        style.ItemSpacing = ImVec2(6.0f, 6.0f);
        style.ChildRounding = 6.0f;
        style.PopupRounding = 6.0f;
        style.FrameRounding = 6.0f;
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        // TODO: move out of here into the window class
        ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), true);
        
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = Context::Get().Instance->GetHandle();
        init_info.PhysicalDevice = Context::Get().PhysicalDevice->GetHandle();
        init_info.Device = Context::Get().Device->GetHandle();
        init_info.QueueFamily = Context::Get().Device->GetGraphicsQueueIndex();
        init_info.Queue = Context::Get().Device->GetGraphicsQueue();
        init_info.DescriptorPool = m_DescriptorPool->GetHandle();
        init_info.RenderPass = m_RenderPass->GetHandle();
        init_info.Subpass = 0;
        init_info.MinImageCount = SwapChain::MAX_FRAMES_IN_FLIGHT;
        init_info.ImageCount = static_cast<uint32_t>(Context::Get().SwapChain->GetImageCount());
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        ImGui_ImplVulkan_Init(&init_info);
        
        // Load Fonts
        ImFontConfig fontConfig;
        fontConfig.FontDataOwnedByAtlas = false;
        ImFont* robotoFont = io.Fonts->AddFontFromMemoryTTF((void*)g_RobotoRegular, sizeof(g_RobotoRegular), 20.0f, &fontConfig);
        ImGuiFonts::Add("Default", robotoFont);
        ImGuiFonts::Add("Bold", io.Fonts->AddFontFromMemoryTTF((void*)g_RobotoBold, sizeof(g_RobotoBold), 20.0f, &fontConfig));
        ImGuiFonts::Add("Italic", io.Fonts->AddFontFromMemoryTTF((void*)g_RobotoItalic, sizeof(g_RobotoItalic), 20.0f, &fontConfig));
        io.FontDefault = robotoFont;
    }

    void ImGuiRenderer::Begin()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiRenderer::End()
    {
        ImGuiIO& io = ImGui::GetIO();
        Application& app = Application::Get();
        io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());
        
        ImGui::Render();
        
        
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    void ImGuiRenderer::Render(CommandBuffer commandBuffer)
    {
        const SwapChain& swapChain = *Context::Get().SwapChain;
        
        static VkClearValue clearValue = { 0.0f, 0.0f, 0.0f, 1.0f };
        
        VkRenderPassBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass = m_RenderPass->GetHandle();
        info.framebuffer = m_FrameBuffers[Context::Get().CurrentImageIndex]->GetHandle();
        info.renderArea.extent.width = swapChain.GetWidth();
        info.renderArea.extent.height = swapChain.GetHeight();
        info.clearValueCount = 1;
        info.pClearValues = &clearValue;
        vkCmdBeginRenderPass(commandBuffer.GetAsVulkanHandle(), &info, VK_SUBPASS_CONTENTS_INLINE);
        
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer.GetAsVulkanHandle());
        
        vkCmdEndRenderPass(commandBuffer.GetAsVulkanHandle());
    }

    void* ImGuiRenderer::GetViewportImage()
    {
        const SwapChain& swapChain = *Context::Get().SwapChain;
        
        // Create the viewport if it doesnt exist
        if (!Context::Get().ImGuiViewport)
        {
            Context::Get().ImGuiViewport = CreateScope<Viewport>(swapChain.GetExtent());
        }
        
        Context::Get().ImGuiViewport->Update(swapChain.GetFrameBuffer(Context::Get().CurrentImageIndex).GetImage());
        
        return Context::Get().ImGuiViewport->GetDescriptorSet();
    }

    void ImGuiRenderer::OnWindowResize(uint32_t width, uint32_t height)
    {
        const SwapChain& swapChain = *Context::Get().SwapChain;
        if (swapChain.GetWidth() == width && swapChain.GetHeight() == height) return;
            
        // recreate framebuffers
        for (int i = 0; i < m_FrameBuffers.size(); i++)
        {
            m_FrameBuffers[i].reset();
            
            VkExtent2D extent = { width, height };
            m_FrameBuffers[i] = CreateScope<FrameBuffer>(nullptr, swapChain.GetFrameBuffer(i).GetImage(), swapChain.GetExtent(), swapChain.GetImageFormat(), m_RenderPass->GetHandle());
        }
    }
}
