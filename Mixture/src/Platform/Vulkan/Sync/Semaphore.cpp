#include "mxpch.hpp"
#include "Platform/Vulkan/Sync/Semaphore.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    Semaphore::Semaphore()
    {
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VK_ASSERT(vkCreateSemaphore(Context::Device->GetHandle(), &semaphoreInfo, nullptr, &m_Semaphore),
                  "Failed to create VkSemaphore!")
    }

    Semaphore::~Semaphore()
    {
        if (m_Semaphore)
        {
            vkDestroySemaphore(Context::Device->GetHandle(), m_Semaphore, nullptr);
            m_Semaphore = nullptr;
        }
    }

    Semaphore::Semaphore(Semaphore&& other) noexcept
        : m_Semaphore(other.m_Semaphore)
    {
        other.m_Semaphore = VK_NULL_HANDLE;
    }

    Semaphore& Semaphore::operator=(Semaphore&& other) noexcept
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
