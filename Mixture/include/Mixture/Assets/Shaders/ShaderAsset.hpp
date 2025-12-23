#pragma once
#include "Mixture/Core/Base.hpp"

#include "Mixture/Assets/IAsset.hpp"

namespace Mixture
{
    class ShaderAsset : public IAsset
    {
    public:
        // We take a generic vector of bytes (the result of DXC compilation)
        ShaderAsset(UUID id, const std::string& name, std::vector<uint8_t> byteCode)
            : m_ID(id), m_Name(name), m_ByteCode(std::move(byteCode))
        {}

        virtual ~ShaderAsset() = default;

        // --- IAsset Interface ---
        UUID GetID() const override { return m_ID; }
        AssetType GetType() const override { return AssetType::Shader; }
        const std::string& GetName() const override { return m_Name; }

        // --- Shader Specific API ---

        // Returns pointer to the start of the DXIL/SPIR-V blob
        const void* GetBufferPointer() const { return m_ByteCode.data(); }

        // Returns size in bytes
        size_t GetBufferSize() const { return m_ByteCode.size(); }

        // Helper: Is this a valid blob?
        bool IsValid() const { return !m_ByteCode.empty(); }

    private:
        UUID m_ID;
        std::string m_Name;
        // Generic storage for DXIL, DXBC, or SPIR-V
        std::vector<uint8_t> m_ByteCode;
    };
}
