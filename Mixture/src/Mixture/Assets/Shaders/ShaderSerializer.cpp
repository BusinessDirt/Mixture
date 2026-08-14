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

        ShaderCompileResult compilation;
        std::vector<uint8_t> compiledBlob;
        std::string ext = metadata.FilePath.extension().string();

        if (ext == ".slang")
        {
            // --- PATH: COMPILE SOURCE ---
            // Construct string from data (careful if not null terminated)
            std::string sourceCode(data.begin(), data.end());

            compilation = ShaderCompiler::CompileDetailed(sourceCode);
            compiledBlob = std::move(compilation.Bytecode);

            if (compiledBlob.empty())
            {
                OPAL_ERROR("AssetManager", "Shader Compilation Failed: {}", metadata.FilePath.string());
                return nullptr;
            }
        }
        else
        {
            compiledBlob.resize(fileSize);
            memcpy(compiledBlob.data(), data.data(), fileSize);
        }

        return CreateRef<ShaderAsset>(metadata.ID, metadata.FilePath.filename().string(),
            std::move(compiledBlob), std::move(compilation.Reflection));
    }
}
