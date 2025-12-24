#pragma once

#include <cstdint>
#include <functional>

namespace Mixture
{
    class UUID
    {
    public:
        // Default constructor generates a new random ID
        UUID();

        // Constructor for creating a UUID from a known value (e.g. loading from disk)
        UUID(uint64_t uuid);

        // Copy constructor
        UUID(const UUID&) = default;

        // Allow explicit conversion to uint64_t
        operator uint64_t() const { return m_UUID; }

        bool operator==(const UUID& other) const { return m_UUID == other.m_UUID; }
        bool operator!=(const UUID& other) const { return m_UUID != other.m_UUID; }

        // Invalid/Null UUID helper
        static UUID Invalid() { return UUID(0); }
        bool IsValid() const { return m_UUID != 0; }

        // TODO: remove once metadata files are implemented
        static UUID FromPath(const std::string& path)
        {
            const char* str = path.c_str();
            // FNV-1a implementation
            uint64_t hash = 14695981039346656037ull;
            while (*str) hash = (hash ^ *str++) * 1099511628211ull;
            return UUID(hash);
        }

    private:
        uint64_t m_UUID;
    };

}

// -----------------------------------------------------------------------------
// std::hash Specialization
// This allows UUID to be used as a key in std::unordered_map
// -----------------------------------------------------------------------------
namespace std
{
    template<>
    struct hash<Mixture::UUID>
    {
        std::size_t operator()(const Mixture::UUID& uuid) const
        {
            return hash<uint64_t>()((uint64_t)uuid);
        }
    };
}
