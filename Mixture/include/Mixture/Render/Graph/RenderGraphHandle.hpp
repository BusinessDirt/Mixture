#pragma once

/**
 * @file RenderGraphHandle.hpp
 * @brief Lightweight handle for RenderGraph resources.
 */

#include "Mixture/Core/Base.hpp"

#include <cstdint>
#include <limits>

namespace Mixture
{

    /**
     * @brief A lightweight handle to a resource in the graph (just an index).
     */
    struct RGResourceHandle 
    {
        using IDType = uint32_t;
        static constexpr IDType InvalidID = std::numeric_limits<IDType>::max();

        IDType ID = InvalidID;

        static RGResourceHandle FromIndex(size_t index)
        {
            return index < InvalidID ? RGResourceHandle{ static_cast<IDType>(index) } : RGResourceHandle{};
        }

        bool IsValid() const { return ID != InvalidID; }
        bool operator==(const RGResourceHandle& other) const { return ID == other.ID; }
        bool operator!=(const RGResourceHandle& other) const { return ID != other.ID; }
    };
}
