#pragma once

/**
 * @file ResourceStates.hpp
 * @brief Definitions for resource states and transitions.
 */

#include "Mixture/Core/Base.hpp"

#include <cstdint>
#include <string_view>

namespace Mixture::RHI
{

    /**
     * @brief Defines the current usage of a GPU resource.
     * Used by the Render Graph to calculate barriers automatically, ensuring proper
     * synchronization and hazard avoidance between different GPU operations.
     */
    enum class ResourceState : uint32_t
    {
        /**
         * @brief The initial state of a resource, or a state where its contents are undefined/discarded.
         */
        Undefined = 0,

        /**
         * @brief Resource is used as a vertex buffer or a constant buffer (Uniform Buffer Object).
         */
        VertexAndConstantBuffer,

        /**
         * @brief Resource is used as an index buffer.
         */
        IndexBuffer,

        /**
         * @brief Resource is used as a render target (color attachment) for writing.
         */
        RenderTarget,

        /**
         * @brief Resource is used for unordered access (read/write) by compute shaders (Shader Storage Buffer Object).
         */
        UnorderedAccess,

        /**
         * @brief Resource is used as a depth/stencil buffer for both reading and writing.
         */
        DepthStencilWrite,

        /**
         * @brief Resource is used as a depth/stencil buffer for read-only operations.
         */
        DepthStencilRead,

        /**
         * @brief Resource is sampled or accessed as a read-only shader resource (e.g., texture, sampled buffer).
         */
        ShaderResource,

        /**
         * @brief Resource is the source in a copy operation (e.g., `vkCmdCopyBuffer`, `vkCmdCopyImage`).
         */
        CopySource,

        /**
         * @brief Resource is the destination in a copy operation.
         */
        CopyDest,

        /**
         * @brief Resource is ready for presentation to the display via a swapchain.
         */
        Present
    };

    /**
     * @brief Checks if a given ResourceState represents a read-only usage.
     *
     * This helper function can be used by the render graph or other synchronization
     * logic to determine if a resource transition requires specific barriers
     * (e.g., read-after-write hazards).
     *
     * @param state The ResourceState to check.
     * @return True if the state is read-only, false otherwise.
     */
    inline bool IsReadOnly(ResourceState state)
    {
        switch (state)
        {
            case ResourceState::VertexAndConstantBuffer:
            case ResourceState::IndexBuffer:
            case ResourceState::DepthStencilRead:
            case ResourceState::ShaderResource:
            case ResourceState::CopySource:
            case ResourceState::Present:
                return true;
            default:
                return false;
        }
    }

    inline std::string_view ToString(ResourceState state)
    {
        switch(state)
        {
            case ResourceState::Undefined: return "Undefined";
            case ResourceState::VertexAndConstantBuffer: return "VertexAndConstantBuffer";
            case ResourceState::IndexBuffer: return "IndexBuffer";
            case ResourceState::RenderTarget: return "RenderTarget";
            case ResourceState::UnorderedAccess: return "UnorderedAccess";
            case ResourceState::DepthStencilWrite: return "DepthStencilWrite";
            case ResourceState::DepthStencilRead: return "DepthStencilRead";
            case ResourceState::ShaderResource: return "ShaderResource";
            case ResourceState::CopySource: return "CopySource";
            case ResourceState::CopyDest: return "CopyDest";
            case ResourceState::Present: return "Present";
            default: return "Undefined";
        }
    }
}
