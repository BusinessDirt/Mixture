#include "mxpch.hpp"
#include "Mixture/Assets/Shaders/ShaderLoader.hpp"

#include "Mixture/Assets/Shaders/ShaderAsset.hpp"

namespace Mixture
{
    Ref<IAsset> ShaderLoader::LoadSync(FileStreamReader& stream, const AssetMetadata& metadata)
    {
        std::vector<char> rawData;
        stream.ReadBuffer(rawData);

        if (rawData.empty()) return nullptr;

        std::vector<uint8_t> compiledBlob;

        // Check Extension to decide: Compile or Just Load?
        std::string ext = metadata.FilePath.extension().string();
        if (ext == ".hlsl")
        {
            // --- PATH: COMPILE SOURCE ---

            // Convert char buffer to string
            std::string sourceCode(rawData.begin(), rawData.end());

            // TODO: Call DXC Compiler here
            // This is where you invoke IDxcCompiler3::Compile
            // For now, we simulate it or return empty.

            // Example:
            // compiledBlob = DxcCompiler::Get().Compile(sourceCode, "vs_6_0", "main");

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
            compiledBlob.resize(rawData.size());
            memcpy(compiledBlob.data(), rawData.data(), rawData.size());
        }

        // Create the Asset
        return CreateRef<ShaderAsset>(metadata.ID, metadata.FilePath.filename().string(), std::move(compiledBlob));
    }
}
