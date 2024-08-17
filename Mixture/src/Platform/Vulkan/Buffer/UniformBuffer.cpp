#include "mxpch.hpp"
#include "UniformBuffer.hpp"

#include "Platform/Vulkan/Context.hpp"
#include "Platform/Vulkan/Descriptor/DescriptorSets.hpp"

namespace Mixture::Vulkan
{
    UniformBuffer::UniformBuffer(const UniformBufferInformation& uboInfo)
        : m_Info(uboInfo)
    {
        m_Buffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < m_Buffers.size(); i++)
        {
            m_Buffers[i] = CreateScope<Buffer>(m_Info.Size, 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            m_Buffers[i]->Map();
        }
        
        // TODO: might need to change this implementation when using other descriptor types as well
        DescriptorSets& descriptorSets = Context::Get().DescriptorSetManager->GetSets();
        for (int i = 0; i < SwapChain::MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorBufferInfo uniformBufferInfo = m_Buffers[i]->GetDescriptorInfo();
            
            const std::vector<VkWriteDescriptorSet> descriptorWrites =
            {
                descriptorSets.Bind(i, uboInfo.Binding, uniformBufferInfo)
            };

            descriptorSets.Update(i, descriptorWrites);
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
