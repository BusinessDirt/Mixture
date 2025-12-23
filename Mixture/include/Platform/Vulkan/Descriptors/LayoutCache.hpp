#pragma once

#include "Platform/Vulkan/Definitions.hpp"
#include "Platform/Vulkan/Device.hpp"

namespace Mixture::Vulkan
{
    class DescriptorLayoutCache
    {
    public:
        DescriptorLayoutCache(Device& device);
        ~DescriptorLayoutCache();

        // The main function: "I need a layout with these bindings"
        vk::DescriptorSetLayout CreateDescriptorLayout(vk::DescriptorSetLayoutCreateInfo* info);

        // Helper struct to use as a Key in the map
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
