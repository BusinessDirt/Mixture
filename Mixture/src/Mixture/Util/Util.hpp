#pragma once

#include <unordered_map>

namespace Mixture
{
    template<typename T, typename ... Rest>
    void HashCombine(std::size_t& seed, const T& v, const Rest&... rest)
    {
        seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
        (HashCombine(seed, rest), ...);
    }

    bool Contains(const std::string& str, const std::string& substr);

    std::string ReadFile(const std::filesystem::path& filepath);
}
