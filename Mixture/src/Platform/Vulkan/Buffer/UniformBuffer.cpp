#include "mxpch.hpp"
#include "UniformBuffer.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Descriptor/DescriptorSets.hpp"

namespace Mixture::Vulkan
{
    UniformBuffer::UniformBuffer(uint32_t size)
    {
        m_Buffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < m_Buffers.size(); i++)
        {
            m_Buffers[i] = CreateScope<Buffer>(size, 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            m_Buffers[i]->Map();
        }
    }

    UniformBuffer::~UniformBuffer()
    {
        for (int i = 0; i < m_Buffers.size(); i++)
        {
            m_Buffers[i] = nullptr;
        }
    }

    void UniformBuffer::Update(void *data)
    {
        m_Buffers[Context::Get().SwapChain->GetCurrentFrameIndex()]->WriteToBuffer(data);
    }
}
