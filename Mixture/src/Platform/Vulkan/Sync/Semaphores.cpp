#include "mxpch.hpp"
#include "Platform/Vulkan/Sync/Semaphores.hpp"

namespace Mixture::Vulkan
{
    Semaphores::Semaphores(vk::Device device, uint32_t count)
        : m_Device(device)
    {
        vk::SemaphoreCreateInfo semaphoreInfo;

        m_Handles.resize(count);
        for (size_t i = 0; i < count; i++)
            m_Handles[i] = m_Device.createSemaphore(semaphoreInfo);
    }

    Semaphores::~Semaphores()
    {
        for (int i = 0; i < m_Handles.size(); i++)
        {
            if (m_Handles[i]) m_Device.destroySemaphore(m_Handles[i]);
        }
    }
}
