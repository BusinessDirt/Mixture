#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class Manager;
    class PhysicalDevice;

    class Device
    {
    public:
        MX_NON_COPIABLE(Device);
        
        Device(const Manager& manager);
        ~Device();
        
        VkFormat FindSupportedFormat( const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        void CreateImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        
        VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
        VkQueue GetPresentQueue() const { return m_PresentQueue; }
        
        uint32_t GetGraphicsQueueIndex() const { return m_GraphicsQueueIndex; }
        uint32_t GetPresentQueueIndex() const { return m_PresentQueueIndex; }
        
    private:
        VULKAN_HANDLE(VkDevice, m_Device);
        
        uint32_t m_GraphicsQueueIndex;
        VkQueue m_GraphicsQueue{VK_NULL_HANDLE};
        
        uint32_t m_PresentQueueIndex;
        VkQueue m_PresentQueue{VK_NULL_HANDLE};
    };
}
