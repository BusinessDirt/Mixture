#include "mxpch.hpp"
#include "Semaphore.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    Semaphore::Semaphore()
    {
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        
        MX_VK_ASSERT(vkCreateSemaphore(Context::Get().Device->GetHandle(), &semaphoreInfo, nullptr, &m_Semaphore),
            "Failed to create VkSemaphore");
    }

    Semaphore::~Semaphore()
    {
        if (m_Semaphore)
        {
            vkDestroySemaphore(Context::Get().Device->GetHandle(), m_Semaphore, nullptr);
            m_Semaphore = nullptr;
        }
    }

    Semaphore::Semaphore(Semaphore&& other)
        : m_Semaphore(other.m_Semaphore)
    {
        other.m_Semaphore = VK_NULL_HANDLE;
    }

    Semaphore& Semaphore::operator=(Semaphore&& other)
    {
        if (this != &other)
        {
            this->~Semaphore();
            m_Semaphore = other.m_Semaphore;
            other.m_Semaphore = VK_NULL_HANDLE;
        }
        return *this;
    }
}
