#pragma once

#include "Mixture/Core/Base.hpp"

#include <vulkan/vulkan.h>

namespace Mixture::Vulkan
{
    class CommandPool
    {
    public:
        MX_NON_COPIABLE(CommandPool);
        
        CommandPool();
        ~CommandPool();
        
    private:
        VULKAN_HANDLE(VkCommandPool, m_CommandPool);
    };
}
