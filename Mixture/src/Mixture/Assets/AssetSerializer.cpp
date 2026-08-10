#include "mxpch.hpp"
#include "Mixture/Assets/AssetSerializer.hpp"

#include <fstream>
#include <sstream>
#include <charconv>

namespace Mixture
{
    std::filesystem::path AssetSerializer::GetMetadataPath(const std::filesystem::path& assetPath)
    {
        return assetPath.string() + ".meta";
    }

    bool AssetSerializer::HasMetadata(const std::filesystem::path& assetPath)
    {
        return std::filesystem::exists(GetMetadataPath(assetPath));
    }

    bool AssetSerializer::TryLoadMetadata(const std::filesystem::path& assetPath, AssetMetadata& outMetadata)
    {
        auto metaPath = GetMetadataPath(assetPath);
        if (!std::filesystem::exists(metaPath)) return false;

        std::ifstream stream(metaPath);
        if (!stream.is_open()) return false;

        AssetMetadata parsed;
        bool foundGUID = false;
        bool foundType = false;
        std::string line;
        while (std::getline(stream, line))
        {
            if (line.empty()) continue;

            size_t delimiterPos = line.find('=');
            if (delimiterPos == std::string::npos) continue;

            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);

            if (key == "GUID")
            {
                uint64_t uuidValue = 0;
                const auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), uuidValue);
                if (foundGUID || error != std::errc{} || end != value.data() + value.size() || uuidValue == 0)
                    return false;
                parsed.ID = UUID(uuidValue);
                foundGUID = true;
            }
            else if (key == "Type")
            {
                unsigned int typeValue = 0;
                const auto [end, error] = std::from_chars(value.data(), value.data() + value.size(), typeValue);
                if (foundType || error != std::errc{} || end != value.data() + value.size()
                    || typeValue <= static_cast<unsigned int>(AssetType::None)
                    || typeValue >= static_cast<unsigned int>(AssetType::Count))
                    return false;
                parsed.Type = static_cast<AssetType>(typeValue);
                foundType = true;
            }
        }

        if (!foundGUID || !foundType || !stream.eof()) return false;
        parsed.FilePath = assetPath;
        outMetadata = std::move(parsed);
        return true;
    }

    void AssetSerializer::WriteMetadata(const AssetMetadata& metadata)
    {
        auto metaPath = GetMetadataPath(metadata.FilePath);
        std::ofstream stream(metaPath);

        if (!stream.is_open())
        {
            OPAL_ERROR("AssetManager", "Failed to write metadata file: {}", metaPath.string());
            return;
        }

        stream << "GUID=" << (uint64_t)metadata.ID << "\n";
        stream << "Type=" << (int)metadata.Type << "\n";

        // Add human readable comments
        stream << "# Asset Type: " << Utils::AssetTypeToString(metadata.Type) << "\n";
    }
}
