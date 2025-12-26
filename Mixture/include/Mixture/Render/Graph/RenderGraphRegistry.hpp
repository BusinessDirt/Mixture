#pragma once

/**
 * @file RenderGraphRegistry.hpp
 * @brief Manages the mapping of virtual handles to physical resources.
 */

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
         * @brief Associates a handle with a real GPU texture.
         *
         * @param handle The virtual handle.
         * @param texture The physical texture resource.
         */
        void RegisterTexture(RGResourceHandle handle, RHI::ITexture* texture);

        /**
         * @brief Retrieves the real GPU texture during pass execution.
         *
         * @param handle The virtual handle.
         * @return RHI::ITexture* Pointer to the physical texture.
         */
        RHI::ITexture* GetTexture(RGResourceHandle handle);

        /** @} */

        /**
         * @name Buffer Management
         * @{
         */

        /**
         * @brief Associates a handle with a real GPU buffer.
         *
         * @param handle The virtual handle.
         * @param buffer The physical buffer resource.
         */
        void RegisterBuffer(RGResourceHandle handle, RHI::IBuffer* buffer);

        /**
         * @brief Retrieves the real GPU buffer during pass execution.
         *
         * @param handle The virtual handle.
         * @return RHI::IBuffer* Pointer to the physical buffer.
         */
        RHI::IBuffer* GetBuffer(RGResourceHandle handle);

        /** @} */

        /**
         * @name External Resources
         * @{
         */

        /**
         * @brief Imports an existing texture (like the Swapchain image) into the graph.
         *
         * @param handle The virtual handle to assign.
         * @param texture The external physical texture.
         */
        void ImportTexture(RGResourceHandle handle, RHI::ITexture* texture);

        /**
         * @brief Imports an existing buffer into the graph.
         *
         * @param handle The virtual handle to assign.
         * @param buffer The external physical buffer.
         */
        void ImportBuffer(RGResourceHandle handle, RHI::IBuffer* buffer);

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
         */
        Vector<RHI::ITexture*> m_Textures;
        
        /**
         * @brief List of buffers indexed by their handle ID.
         */
        Vector<RHI::IBuffer*>  m_Buffers;
    };

}
