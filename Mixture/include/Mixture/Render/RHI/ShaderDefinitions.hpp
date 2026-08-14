#pragma once

#include "Mixture/Core/Base.hpp"

namespace Mixture::RHI
{
    /**
     * @brief Defines the programmable stage of the rendering pipeline that a shader belongs to.
     */
    enum class ShaderStage
    {
        /**
         * @brief The vertex shader stage, processing individual vertices.
         */
        Vertex = 0x00000001,

        /**
         * @brief The fragment shader stage, processing fragments (pixels).
         */
        Fragment = 0x00000010,

        /**
         * @brief The compute shader stage, for general-purpose computing.
         */
        Compute = 0x00000020
    };

    /**
     * @brief Stable identity used to cache objects derived from shader code.
     *
     * StableID identifies the logical shader asset, while Version changes each
     * time that asset is reloaded. Stage distinguishes entry points within it.
     */
    struct ShaderIdentity
    {
        uint64_t StableID = 0;
        uint64_t Version = 0;
        ShaderStage Stage = ShaderStage::Vertex;

        bool operator==(const ShaderIdentity&) const = default;
        explicit operator bool() const { return StableID != 0 && Version != 0; }
    };
}
