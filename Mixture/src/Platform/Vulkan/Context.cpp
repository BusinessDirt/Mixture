#include "mxpch.hpp"
#include "Context.hpp"

#include "Platform/Vulkan/Buffer/FrameBuffer.hpp"
#include "Mixture/Assets/Shaders/ShaderDescriptors.hpp"

#ifdef MX_PLATFORM_MACOSX
    #include <vulkan/vulkan_metal.h>
#endif

#ifdef MX_PLATFORM_WINDOWS
    #include <vulkan/vulkan_win32.h>
#endif

namespace Mixture::Vulkan
{
    namespace Util
    {
        static std::vector<const char*> GetRequiredLayers()
        {
            std::vector<const char*> requiredLayers;

            // Common layers for validation
#ifndef MX_DIST
            requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

            return requiredLayers;
        }

        static std::vector<const char*> GetRequiredInstanceExtensions()
        {
            std::vector<const char*> requiredExtensions;

            // Common extensions
            requiredExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

#if defined(MX_PLATFORM_WINDOWS)
            requiredExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(MX_PLATFORM_MACOSX)
            requiredExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
            requiredExtensions.push_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
#elif defined(MX_PLATFORM_LINUX)
            requiredExtensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#endif

#ifndef MX_DIST
            requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

            return requiredExtensions;
        }

        static std::vector<const char*> GetRequiredDeviceExtensions()
        {
            std::vector<const char*> requiredExtensions;

            // Swapchain
            requiredExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

#ifdef OPAL_PLATFORM_MACOSX
            // Required for MoltenVK
            requiredExtensions.push_back("VK_KHR_portability_subset");
#endif

            return requiredExtensions;
        }

        static Vector<Vulkan::DescriptorBinding> ConvertToVulkanBindings(const DescriptorLayout& layout)
        {
            Vector<Vulkan::DescriptorBinding> vulkanBindings{};
            for (const auto& [binding, descriptor] : layout.Elements)
            {
                Vulkan::DescriptorBinding vulkanBinding{};
                vulkanBinding.Binding = binding;
                vulkanBinding.DescriptorCount = 1;
                vulkanBinding.Stage = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;
                vulkanBinding.Type = static_cast<VkDescriptorType>(descriptor.Type);
                vulkanBindings.emplace_back(vulkanBinding);
            }
            return vulkanBindings;
        }
    }

    Scope<Context> Context::s_Instance = nullptr;
    std::mutex Context::s_Mutex;

    Context& Context::Get()
    {
        std::lock_guard<std::mutex> lock(s_Mutex);
        if (s_Instance == nullptr) s_Instance = CreateScope<Context>();
        return *s_Instance;
    }

    void Context::Initialize(const std::string& applicationName)
    {
        m_Instance = CreateScope<Instance>(applicationName, Util::GetRequiredLayers(), Util::GetRequiredInstanceExtensions());

#ifndef MX_DIST
        m_DebugMessenger = CreateScope<DebugMessenger>();
#endif
        
        m_Surface = CreateScope<Surface>();
        m_PhysicalDevice = CreateScope<PhysicalDevice>(Util::GetRequiredDeviceExtensions());
        m_Device = CreateScope<Device>(Util::GetRequiredLayers(), Util::GetRequiredDeviceExtensions());
        m_CommandPool = CreateScope<CommandPool>();
        m_SwapChain = CreateScope<SwapChain>();

        // assume set 0 is global and set 1 is instance
        Vector<DescriptorLayout> defaultLayouts = GetDefaultLayouts();
        m_GlobalDescriptorSet = CreateScope<DescriptorSetManager>(Util::ConvertToVulkanBindings(defaultLayouts[0]), SwapChain::MAX_FRAMES_IN_FLIGHT);
        m_InstanceDescriptorSet = CreateScope<DescriptorSetManager>(Util::ConvertToVulkanBindings(defaultLayouts[1]), SwapChain::MAX_FRAMES_IN_FLIGHT);
    }

    void Context::Shutdown()
    {
        if (m_ImGuiViewport) m_ImGuiViewport = nullptr;

        m_GlobalDescriptorSet = nullptr;
        m_InstanceDescriptorSet = nullptr;

        m_CommandPool = nullptr;
        m_SwapChain = nullptr;
        m_Device = nullptr;
        m_PhysicalDevice = nullptr;
        m_DebugMessenger = nullptr;
        m_Surface = nullptr;
        m_Instance = nullptr;
    }

    void Context::RebuildSwapChain()
    {
        Ref<SwapChain> oldSwapChain = std::move(m_SwapChain);
        m_SwapChain = CreateScope<SwapChain>(oldSwapChain);
        MX_CORE_ASSERT(oldSwapChain->CompareSwapFormats(*m_SwapChain),
            "Swap chain image(or depth) format has changed!");
    }

    void Context::CreateImGuiViewport()
    {
        if (!m_ImGuiViewport) m_ImGuiViewport = CreateScope<Viewport>(m_SwapChain->GetExtent());
    }
}
