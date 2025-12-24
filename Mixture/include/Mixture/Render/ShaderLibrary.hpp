#pragma once

#include "Mixture/Render/RHI/IPipeline.hpp"
#include "Mixture/Render/RHI/IGraphicsDevice.hpp"
#include "Mixture/Assets/AssetManager.hpp"

#include <unordered_map>

namespace Mixture
{
    /**
     * @brief Manages the lifecycle of RHI shader objects and maps them to AssetHandles.
     */
    class ShaderLibrary
    {
    public:
        static void Init(RHI::IGraphicsDevice& device);
        static void Shutdown();

        /**
         * @brief Retrieves an RHI shader for the given asset handle.
         * Creates the shader if it doesn't exist in the library.
         */
        static RHI::IShader* GetShader(AssetHandle handle, RHI::ShaderStage stage);

        /**
         * @brief Forces a reload of a shader from its asset.
         */
        static void Reload(AssetHandle handle);

        /**
         * @brief Clears all cached shaders.
         */
        static void Clear();

    private:
        static RHI::IGraphicsDevice* s_Device;
        static std::unordered_map<UUID, Ref<RHI::IShader>> s_Cache;
    };
}
