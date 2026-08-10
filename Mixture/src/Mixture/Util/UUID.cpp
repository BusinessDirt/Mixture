#include "mxpch.hpp"
#include "Mixture/Util/UUID.hpp"

#include <random>
#include <mutex>

namespace Mixture
{

    // Initialize the random engine once (static)
    static std::random_device s_RandomDevice;
    static std::mt19937_64 s_Engine(s_RandomDevice());
    static std::uniform_int_distribution<uint64_t> s_UniformDistribution;
    static std::mutex s_EngineMutex;

    UUID::UUID()
    {
        std::lock_guard<std::mutex> lock(s_EngineMutex);
        do
        {
            m_UUID = s_UniformDistribution(s_Engine);
        } while (m_UUID == 0);
    }

    UUID::UUID(uint64_t uuid)
        : m_UUID(uuid)
    {}
}
