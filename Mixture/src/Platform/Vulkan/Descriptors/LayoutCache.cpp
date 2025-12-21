#include "mxpch.hpp"
#include "Platform/Vulkan/Descriptors/LayoutCache.hpp"

#include "Mixture/Util/Util.hpp"

namespace Mixture::Vulkan
{
    void DescriptorLayoutCache::Init(vk::Device device)
    {
        m_Device = device;
    }

    void DescriptorLayoutCache::Shutdown()
    {
        // Destroy all cached layouts
        for (auto& pair : m_LayoutCache)
            m_Device.destroyDescriptorSetLayout(pair.second);

        m_LayoutCache.clear();
    }

    vk::DescriptorSetLayout DescriptorLayoutCache::CreateDescriptorLayout(vk::DescriptorSetLayoutCreateInfo* info)
    {
        DescriptorLayoutInfo layoutInfo;
        layoutInfo.Bindings.reserve(info->bindingCount);
        bool isSorted = true;
        int32_t lastBinding = -1;

        // Copy bindings from the CreateInfo into our Key struct
        for (uint32_t i = 0; i < info->bindingCount; i++)
        {
            layoutInfo.Bindings.push_back(info->pBindings[i]);

            // Check for sorting while we copy
            if ((int32_t)info->pBindings[i].binding > lastBinding)
            {
                lastBinding = info->pBindings[i].binding;
            }
            else
            {
                isSorted = false;
            }
        }

        // Sort them so the cache key is consistent regardless of insertion order
        if (!isSorted)
        {
            std::sort(layoutInfo.Bindings.begin(), layoutInfo.Bindings.end(),
                [](const vk::DescriptorSetLayoutBinding& a, const vk::DescriptorSetLayoutBinding& b) {
                    return a.binding < b.binding;
                });
        }

        // Try to find in cache
        auto it = m_LayoutCache.find(layoutInfo);
        if (it != m_LayoutCache.end()) return it->second;

        // Create new layout (using the provided info, which might point to pBindings in the struct)
        // Note: We use the sorted list from layoutInfo to ensure consistency if we were rebuilding info,
        // but typically passing the original info is fine if the contents match.
        // However, strictly speaking, we should pass the sorted bindings to Vulkan.

        vk::DescriptorSetLayoutCreateInfo newInfo = *info;
        newInfo.pBindings = layoutInfo.Bindings.data(); // Use the sorted list
        newInfo.bindingCount = (uint32_t)layoutInfo.Bindings.size();

        vk::DescriptorSetLayout layout;
        try
        {
            layout = m_Device.createDescriptorSetLayout(newInfo);
        }
        catch (vk::SystemError& err)
        {
            OPAL_ERROR("Core/Vulkan", "DescriptorLayoutCache - Failed to create Descriptor Set Layout!");
            return nullptr;
        }

        // Store in cache
        m_LayoutCache[layoutInfo] = layout;
        return layout;
    }

    bool DescriptorLayoutCache::DescriptorLayoutInfo::operator==(const DescriptorLayoutInfo& other) const
    {
        if (other.Bindings.size() != Bindings.size()) return false;

        for (size_t i = 0; i < Bindings.size(); i++)
        {
            if (other.Bindings[i].binding != Bindings[i].binding) return false;
            if (other.Bindings[i].descriptorType != Bindings[i].descriptorType) return false;
            if (other.Bindings[i].descriptorCount != Bindings[i].descriptorCount) return false;
            if (other.Bindings[i].stageFlags != Bindings[i].stageFlags) return false;
        }
        return true;
    }

    size_t DescriptorLayoutCache::DescriptorLayoutInfo::Hash() const
    {
        std::size_t seed = 0;
        for (const auto& b : Bindings)
        {
            Util::HashCombine(seed, b.binding, static_cast<uint32_t>(b.descriptorType),
                b.descriptorCount, static_cast<uint32_t>(b.stageFlags));
        }
        return seed;
    }
}
