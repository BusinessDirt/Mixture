#include "mxpch.hpp"
#include "Mixture/Assets/AssetSerializer.hpp"

#include <fstream>
#include <sstream>

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
                uint64_t uuidVal = std::stoull(value);
                outMetadata.ID = UUID(uuidVal);
            }
            else if (key == "Type")
            {
                // Simple integer parsing for now, ideally string to enum
                int typeVal = std::stoi(value);
                outMetadata.Type = (AssetType)typeVal;
            }
        }

        outMetadata.FilePath = assetPath;
        return true;
    }

    void AssetSerializer::WriteMetadata(const AssetMetadata& metadata)
    {
        auto metaPath = GetMetadataPath(metadata.FilePath);
        std::ofstream stream(metaPath);

        if (!stream.is_open())
        {
            OPAL_ERROR("Core/Assets", "Failed to write metadata file: {}", metaPath.string());
            return;
        }

        stream << "GUID=" << (uint64_t)metadata.ID << "\n";
        stream << "Type=" << (int)metadata.Type << "\n";
        
        // Add human readable comments
        stream << "# Asset Type: " << Utils::AssetTypeToString(metadata.Type) << "\n";
    }
}
