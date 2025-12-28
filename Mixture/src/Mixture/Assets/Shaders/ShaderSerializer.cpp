#include "mxpch.hpp"
#include "Mixture/Assets/Shaders/ShaderSerializer.hpp"

#include "Mixture/Assets/Shaders/ShaderAsset.hpp"
#include "Mixture/Assets/Shaders/ShaderCompiler.hpp"

namespace Mixture
{
    Ref<IAsset> ShaderSerializer::Load(const Vector<char>& data, const AssetMetadata& metadata)
    {
        if (data.empty()) return nullptr;

        size_t fileSize = data.size();

        std::vector<uint8_t> compiledBlob;
        std::string ext = metadata.FilePath.extension().string();

        if (ext == ".hlsl")
        {
            // --- PATH: COMPILE SOURCE ---
            // Construct string from data (careful if not null terminated)
            std::string sourceCode(data.begin(), data.end());

            compiledBlob = ShaderCompiler::Compile(sourceCode);

            if (compiledBlob.empty())
            {
                OPAL_ERROR("Core/Assets", "Shader Compilation Failed: {}", metadata.FilePath.string());
                return nullptr;
            }
        }
        else
        {
            // --- PATH: LOAD BINARY ---
            // File is already .cso / .spv (pre-compiled)
            compiledBlob.resize(fileSize);
            memcpy(compiledBlob.data(), data.data(), fileSize);
        }

        // Create the Asset
        return CreateRef<ShaderAsset>(metadata.ID, metadata.FilePath.filename().string(), std::move(compiledBlob));
    }
}
