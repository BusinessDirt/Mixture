#include "mxpch.hpp"
#include "Context.hpp"

namespace Mixture::Vulkan
{
    Context* Context::s_Instance = nullptr;
    std::mutex Context::s_Mutex;

    Context& Context::Get()
    {
        std::lock_guard<std::mutex> lock(s_Mutex);
        if (s_Instance == nullptr) s_Instance = new Context();
        return *s_Instance;
    }
}
