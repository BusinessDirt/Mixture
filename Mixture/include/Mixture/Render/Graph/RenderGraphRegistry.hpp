#pragma once

#include "Mixture/Core/Base.hpp"

#include "Mixture/Render/Graph/RenderGraphHandle.hpp"
#include "Mixture/Render/RHI/ITexture.hpp"
#include "Mixture/Render/RHI/IBuffer.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <cassert>

namespace Mixture 
{

    /**
     * @brief Manages the mapping between virtual resource handles and physical GPU resources.
     */
    class RenderGraphRegistry 
    {
    public:
        RenderGraphRegistry() = default;
        ~RenderGraphRegistry() = default;

        /**
         * @name Texture Management
         * @{
         */
        
        /**
         * @brief Associates a handle with a real GPU resource.
         * 
         * @param handle The virtual handle.
         * @param texture The physical texture resource.
         */
        void RegisterTexture(RGResourceHandle handle, Ref<RHI::ITexture> texture);

        /**
         * @brief Retrieves the real GPU resource during pass execution.
         * This is the function you call inside your Execute lambda!
         * 
         * @param handle The virtual handle.
         * @return RHI::ITexture* Pointer to the physical texture.
         */
        RHI::ITexture* GetTexture(RGResourceHandle handle);
        
        /** @} */

        /**
         * @name External Resources
         * @{
         */
        
        /**
         * @brief Imports an existing texture (like the Swapchain image) into the graph.
         * Returns a handle that passes can use to read/write to it.
         * 
         * @param handle The virtual handle to assign.
         * @param texture The external physical texture.
         */
        void ImportTexture(RGResourceHandle handle, Ref<RHI::ITexture> texture);

        /** @} */

        /**
         * @name Maintenance
         * @{
         */
        
        /**
         * @brief Clears the registry, releasing references to all resources.
         */
        void Clear();

        /** @} */

    private:
        /**
         * @brief List of textures indexed by their handle ID.
         * We use a vector because handles are just indices (0, 1, 2...), providing O(1) lookup speed.
         */
        Vector<Ref<RHI::ITexture>> m_Textures;
        Vector<Ref<RHI::IBuffer>>  m_Buffers;
    };

}
