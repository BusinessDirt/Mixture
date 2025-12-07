#pragma once

#include <vulkan/vulkan.h>

#include <string>

namespace Mixture::Vulkan::ToString
{

    /**
     * @brief Formats VkFormat to string view.
     */
    std::string_view Format(VkFormat format);

    /**
     * @brief Formats VkFormat to string view for shader usage.
     */
    std::string_view ShaderFormat(VkFormat format);

    /**
     * @brief Formats VkColorSpaceKHR to string view.
     */
    std::string_view ColorSpace(VkColorSpaceKHR colorSpace);

    /**
     * @brief Formats VkPresentModeKHR to string view.
     */
    std::string_view PresentMode(VkPresentModeKHR presentMode);

    /**
     * @brief Formats VkPhysicalDeviceType to string view.
     */
    std::string_view PhysicalDeviceType(VkPhysicalDeviceType physicalDeviceType);

    /**
     * @brief Formats VkDescriptorType to string view.
     */
    std::string_view DescriptorType(VkDescriptorType descriptorType);

    /**
     * @brief Formats Vulkan version to string.
     */
    std::string Version(uint32_t version);
}
