#include "mxpch.hpp"
#include "Platform/Vulkan/Buffer/Uniform.hpp"

namespace Mixture::Vulkan
{
    UniformBuffer::UniformBuffer(size_t instanceSize, uint32_t instanceCount)
    {
        m_Buffer = CreateScope<Buffer>(instanceSize, instanceCount, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        m_DescriptorInfo = {};
    }

    void UniformBuffer::SetData(const void* data, const uint32_t index) const
    {
        const uint32_t size = static_cast<uint32_t>(m_Buffer->GetInstanceSize());
        m_Buffer->Map();
        m_Buffer->WriteToBuffer(data, size, static_cast<VkDeviceSize>(size) * index);
        m_Buffer->Unmap();
    }

    const void* UniformBuffer::GetDescriptorInfo(const uint32_t index)
    {
        const uint32_t size = static_cast<uint32_t>(m_Buffer->GetInstanceSize());
        m_DescriptorInfo.buffer = m_Buffer->GetHandle();
        m_DescriptorInfo.offset = static_cast<VkDeviceSize>(size) * index;
        m_DescriptorInfo.range  = size;
        return &m_DescriptorInfo;
    }
}


