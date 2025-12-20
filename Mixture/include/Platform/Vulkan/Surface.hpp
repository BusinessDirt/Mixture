#pragma once
#include "Mixture/Core/Base.hpp"
#include "Platform/Vulkan/Instance.hpp"

#include <vulkan/vulkan.hpp>

namespace Mixture::Vulkan
{
    class Surface
    {
    public:
        Surface(Ref<Instance> instance, void* windowHandle);
        ~Surface();

        vk::SurfaceKHR GetHandle() const { return m_Handle; }

    private:
        Ref<Instance> m_Instance;
        vk::SurfaceKHR m_Handle;
    };
}
