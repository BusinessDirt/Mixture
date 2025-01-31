#include "mxpch.hpp"
#include "Fence.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Mixture::Vulkan
{
    Fence::Fence(bool signaled)
    {
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
        
        MX_VK_ASSERT(vkCreateFence(Context::Get().GetDevice().GetHandle(), &fenceInfo, nullptr, &m_Fence),
            "Failed to create VkFence");
    }

    Fence::~Fence()
    {
        if (m_Fence)
        {
            vkDestroyFence(Context::Get().GetDevice().GetHandle(), m_Fence, nullptr);
            m_Fence = nullptr;
        }
    }

    Fence::Fence(Fence&& other)
        : m_Fence(other.m_Fence)
    {
        other.m_Fence = VK_NULL_HANDLE;
    }

    Fence& Fence::operator=(Fence&& other)
    {
        if (this != &other)
        {
            this->~Fence();
            m_Fence = other.m_Fence;
            other.m_Fence = VK_NULL_HANDLE;
        }
        return *this;
    }

    void Fence::Wait(uint64_t timeout)
    {
        vkWaitForFences(Context::Get().GetDevice().GetHandle(), 1, &m_Fence, VK_TRUE, timeout);
    }

    void Fence::Reset()
    {
        vkResetFences(Context::Get().GetDevice().GetHandle(), 1, &m_Fence);
    }
}
