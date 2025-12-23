#include "mxpch.hpp"
#include "Platform/Vulkan/Sync/Fences.hpp"

namespace Mixture::Vulkan
{
    Fences::Fences(Device& device, uint32_t count, bool signaled)
        : m_Device(&device)
    {
        vk::FenceCreateFlags flags;
        if (signaled) flags = vk::FenceCreateFlagBits::eSignaled;
        vk::FenceCreateInfo fenceInfo(flags);

        m_Handles.resize(count);
        for (int i = 0; i < count; i++)
            m_Handles[i] = m_Device->GetHandle().createFence(fenceInfo);
    }

    Fences::~Fences()
    {
        for (int i = 0; i < m_Handles.size(); i++)
        {
            if (m_Handles[i]) m_Device->GetHandle().destroyFence(m_Handles[i]);
        }
    }

    vk::Result Fences::Wait(uint32_t index)
    {
        return m_Device->GetHandle().waitForFences(1, &m_Handles[index], VK_TRUE, UINT64_MAX);
    }

    vk::Result Fences::Reset(uint32_t index)
    {
        return m_Device->GetHandle().resetFences(1, &m_Handles[index]);
    }
}
