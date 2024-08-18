#include "mxpch.hpp"
#include "ImGuiRenderer.hpp"

#include "Mixture/Core/Application.hpp"

#include "Platform/Vulkan/Context.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

namespace Mixture::Vulkan
{
    ImGuiRenderer::ImGuiRenderer() {}

    ImGuiRenderer::~ImGuiRenderer()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        
        m_DescriptorPool = nullptr;
    }

    void ImGuiRenderer::Init()
    {
        m_DescriptorPool = DescriptorPool::CreateImGuiDescriptorPool();
        
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
        //io.ConfigViewportsNoAutoMerge = true;
        //io.ConfigViewportsNoTaskBarIcon = true;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
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
        init_info.RenderPass = Context::Get().SwapChain->GetRenderPass().GetHandle();
        init_info.Subpass = 0;
        init_info.MinImageCount = SwapChain::MAX_FRAMES_IN_FLIGHT;
        init_info.ImageCount = static_cast<uint32_t>(Context::Get().SwapChain->GetImageCount());
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        ImGui_ImplVulkan_Init(&init_info);
    }

    void ImGuiRenderer::BeginFrame(CommandBuffer commandBuffer)
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiRenderer::EndFrame(CommandBuffer commandBuffer)
    {
        ImGuiIO& io = ImGui::GetIO();
        Application& app = Application::Get();
        io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());
        
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer.GetAsVulkanHandle());
        
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }
}
