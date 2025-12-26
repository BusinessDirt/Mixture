#pragma once

/**
 * @file ShaderAsset.hpp
 * @brief Asset representation of a compiled shader.
 */

#include "Mixture/Core/Base.hpp"

#include "Mixture/Assets/IAsset.hpp"

namespace Mixture
{
    /**
     * @brief Represents a shader asset containing compiled bytecode.
     */
    class ShaderAsset : public IAsset
    {
    public:
        /**
         * @brief Constructs a ShaderAsset.
         * 
         * @param id The asset ID.
         * @param name The asset name.
         * @param byteCode The compiled shader bytecode (SPIR-V, DXIL, etc.).
         */
        ShaderAsset(UUID id, const std::string& name, std::vector<uint8_t> byteCode)
            : m_ID(id), m_Name(name), m_ByteCode(std::move(byteCode))
        {}

        virtual ~ShaderAsset() = default;

        // --- IAsset Interface ---
        UUID GetID() const override { return m_ID; }
        AssetType GetType() const override { return AssetType::Shader; }
        const std::string& GetName() const override { return m_Name; }

        // --- Shader Specific API ---

        /**
         * @brief Gets a pointer to the start of the bytecode blob.
         * 
         * @return const void* Pointer to the data.
         */
        const void* GetBufferPointer() const { return m_ByteCode.data(); }

        /**
         * @brief Gets the size of the bytecode blob in bytes.
         * 
         * @return size_t Size in bytes.
         */
        size_t GetBufferSize() const { return m_ByteCode.size(); }

        /**
         * @brief Checks if the shader asset is valid (has bytecode).
         * 
         * @return true If valid.
         */
        bool IsValid() const { return !m_ByteCode.empty(); }

    private:
        UUID m_ID;
        std::string m_Name;
        // Generic storage for DXIL, DXBC, or SPIR-V
        std::vector<uint8_t> m_ByteCode;
    };
}
