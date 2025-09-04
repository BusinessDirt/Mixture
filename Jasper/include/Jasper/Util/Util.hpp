#pragma once

#include <filesystem>

namespace Jasper::Util
{
    std::string ReadFile(const std::filesystem::path& filepath);
    void HashCombine(std::size_t& seed, std::size_t value);
}
