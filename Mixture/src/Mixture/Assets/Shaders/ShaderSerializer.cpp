#include "mxpch.hpp"
#include "Mixture/Assets/Shaders/ShaderSerializer.hpp"

#include "Mixture/Assets/Shaders/ShaderAsset.hpp"
#include "Mixture/Assets/Shaders/ShaderCompiler.hpp"

namespace Mixture
{
    Ref<IAsset> ShaderSerializer::Load(FileStreamReader& stream, const AssetMetadata& metadata, ArenaAllocator* allocator)
    {
        size_t fileSize = stream.GetFileSize();
        if (fileSize == 0) return nullptr;

        void* rawBuffer = nullptr;
        std::vector<char> heapBuffer;

        // Optimization: Use Arena for temporary file read buffer if available
        if (allocator)
        {
            rawBuffer = allocator->AllocRaw(fileSize + 1); // +1 for null terminator safety
            ((char*)rawBuffer)[fileSize] = '\0';
        }
        else
        {
            heapBuffer.resize(fileSize + 1);
            rawBuffer = heapBuffer.data();
            heapBuffer[fileSize] = '\0';
        }

        if (!rawBuffer) return nullptr;

        if (!stream.ReadRaw(rawBuffer, fileSize))
        {
            OPAL_ERROR("Core/Assets", "Failed to read shader file: {}", metadata.FilePath.string());
            return nullptr;
        }

        std::vector<uint8_t> compiledBlob;
        std::string ext = metadata.FilePath.extension().string();

        if (ext == ".hlsl")
        {
            // --- PATH: COMPILE SOURCE ---
            // rawBuffer is essentially a string now
            std::string sourceCode((char*)rawBuffer, fileSize);

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
            memcpy(compiledBlob.data(), rawBuffer, fileSize);
        }

        // Create the Asset
        return CreateRef<ShaderAsset>(metadata.ID, metadata.FilePath.filename().string(), std::move(compiledBlob));
    }
}
