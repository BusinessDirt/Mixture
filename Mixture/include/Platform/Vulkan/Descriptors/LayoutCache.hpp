#pragma once

/**
 * @file LayoutCache.hpp
 * @brief Caching mechanism for Vulkan Descriptor Set Layouts.
 */

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Device.hpp"

namespace Mixture::Vulkan
{
    /**
     * @brief Caches Descriptor Set Layouts to prevent redundant creation.
     *
     * Stores created layouts in a hash map and returns existing ones if requested again.
     */
    class DescriptorLayoutCache
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param device Reference to the logical device.
         */
        DescriptorLayoutCache(Device& device);
        ~DescriptorLayoutCache();

        /**
         * @brief Creates or retrieves a descriptor set layout.
         *
         * @param info Creation info for the descriptor set layout.
         * @return vk::DescriptorSetLayout The created or cached layout.
         */
        vk::DescriptorSetLayout CreateDescriptorLayout(vk::DescriptorSetLayoutCreateInfo* info);

        /**
         * @brief Helper struct to serve as a key in the layout cache map.
         */
        struct DescriptorLayoutInfo
        {
            Vector<vk::DescriptorSetLayoutBinding> Bindings;

            bool operator==(const DescriptorLayoutInfo& other) const;
            size_t Hash() const;
        };

    private:
        struct DescriptorLayoutHash
        {
            std::size_t operator()(const DescriptorLayoutInfo& k) const
            {
                return k.Hash();
            }
        };

        std::unordered_map<DescriptorLayoutInfo, vk::DescriptorSetLayout, DescriptorLayoutHash> m_LayoutCache;
        Device* m_Device;
    };
}
