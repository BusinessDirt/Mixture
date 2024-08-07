#pragma once

#include <vulkan/vulkan.h>

struct VulkanContext
{
    VkInstance Instance = VK_NULL_HANDLE;
    VkAllocationCallbacks* Allocator = nullptr;
};
