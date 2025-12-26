#pragma once

/**
 * @file IAsset.hpp
 * @brief Base definitions for the asset system.
 */

#include "Mixture/Core/Base.hpp"

#include "Mixture/Util/UUID.hpp"

namespace Mixture
{
    /**
     * @brief Enum representing supported asset types.
     */
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

    /**
     * @brief Metadata associated with an asset.
     */
    struct AssetMetadata
    {
        UUID ID = 0;
        AssetType Type = AssetType::None;
        std::filesystem::path FilePath;

        //bool IsLoaded = false;
        //bool IsMemoryOnly = false;
    };

    /**
     * @brief Handle to an asset, wrapping its UUID and a validation magic number.
     */
    struct AssetHandle
    {
        UUID ID;
        uint32_t Magic = 0; // Magic number to validate against stale memory/reloads

        bool operator==(const AssetHandle& other) const
        {
            return ID == other.ID && Magic == other.Magic;
        }

        bool operator!=(const AssetHandle& other) const
        {
            return !(*this == other);
        }

        operator bool() const { return ID.IsValid() && Magic != 0; }
    };

    /**
     * @brief Interface for any asset resource.
     */
    class IAsset
    {
    public:
        virtual ~IAsset() = default;

        /**
         * @brief Gets the unique identifier for this specific asset instance.
         * 
         * @return UUID The asset ID.
         */
        virtual UUID GetID() const = 0;

        /**
         * @brief Gets the type of this asset.
         * 
         * @return AssetType The asset type.
         */
        virtual AssetType GetType() const = 0;

        /**
         * @brief Gets the debug name of the asset (usually the filename).
         * 
         * @return const std::string& The name.
         */
        virtual const std::string& GetName() const = 0;

        /**
         * @brief Gets the magic number (generation) of this asset instance.
         */
        uint32_t GetMagic() const { return m_Magic; }

        /**
         * @brief Sets the magic number. Should be managed by AssetManager.
         */
        void SetMagic(uint32_t magic) { m_Magic = magic; }

    protected:
        uint32_t m_Magic = 0;
    };

    namespace Utils
    {
        inline const char* AssetTypeToString(AssetType type)
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
