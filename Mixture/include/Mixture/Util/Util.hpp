#pragma once

/**
 * @file Util.hpp
 * @brief General utility functions (hashing, string manipulation, file I/O).
 */

#include <unordered_map>
#include <filesystem>

namespace Mixture::Util
{
    /**
     * @brief Combines a hash with a new value.
     * 
     * @tparam T Type of the value to combine.
     * @tparam Rest Remaining types.
     * @param seed The current hash seed.
     * @param v The value to hash and combine.
     * @param rest Remaining values.
     */
    template<typename T, typename ... Rest>
    void HashCombine(std::size_t& seed, const T& v, const Rest&... rest)
    {
        seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
        (HashCombine(seed, rest), ...);
    }

    /**
     * @brief Checks if a string contains a substring.
     * 
     * @param str The string to search in.
     * @param substr The substring to search for.
     * @return true If str contains substr.
     */
    bool Contains(const std::string& str, const std::string& substr);

    /**
     * @brief Reads a file into a string.
     * 
     * @param filepath The path to the file.
     * @return std::string The content of the file.
     */
    std::string ReadFile(const std::filesystem::path& filepath);
}
