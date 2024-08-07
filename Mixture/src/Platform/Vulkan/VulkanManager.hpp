#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture
{
    class VulkanManager
    {
    public:
        VulkanManager();
        
        void Init();
        
        const std::vector<const char*>& GetLayers() const { return m_Layers; }
        const std::vector<const char*>& GetExtensions() const { return m_Extensions; }
        const std::vector<VkLayerSettingEXT>& GetLayerSettings() const { return m_LayerSettings; }
        
        bool CheckLayerSupport();
        bool CheckExtensionSupport();
        
    private:
        std::vector<VkLayerProperties> GetAvailableLayers() const;
        std::vector<VkExtensionProperties> GetAvailableExtensions() const;
        
        void GetRequiredLayers();
        void GetRequiredExtensions();
        
    private:
        std::vector<const char*> m_Layers{};
        std::vector<const char*> m_Extensions{};
        std::vector<VkLayerSettingEXT> m_LayerSettings{};
    };
}
