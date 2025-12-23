#pragma once
#include "Mixture/Core/Base.hpp"

#include "Mixture/Util/UUID.hpp"
#include "Mixture/Assets/AssetManager.hpp"

namespace Mixture
{
    enum class AssetType : uint8_t
    {
        None = 0,
        Texture,
        Mesh,
        Shader,
        Material,
        Scene,
        Count // Helper for array sizing
    };

    struct AssetMetadata
    {
        UUID ID = 0;
        AssetType Type = AssetType::None;
        std::filesystem::path FilePath;

        //bool IsLoaded = false;
        //bool IsMemoryOnly = false;
    };

    template<typename T>
    struct AssetHandle
    {
        UUID ID;
    };

    class IAsset
    {
    public:
        virtual ~IAsset() = default;

        // Unique identifier for this specific asset instance
        virtual UUID GetID() const = 0;

        // What kind of asset is this? (Helps with casting)
        virtual AssetType GetType() const = 0;

        // Debug name (usually the filename)
        virtual const std::string& GetName() const = 0;
    };

    namespace Utils
    {
        const char* AssetTypeToString(AssetType type)
        {
            switch (type)
            {
                case AssetType::None: return "None";
                case AssetType::Texture: return "Texture";
                case AssetType::Mesh: return "Mesh";
                case AssetType::Shader: return "Shader";
                case AssetType::Material: return "Material";
                case AssetType::Scene: return "Scene";
                case AssetType::Count: return "Count";
            }

            return "Unknown";
        }
    }
}
