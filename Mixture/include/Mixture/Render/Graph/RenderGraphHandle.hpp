#pragma once

#include "Mixture/Core/Base.hpp"

#include <cstdint>
#include <limits>

namespace Mixture
{

    // A lightweight handle to a resource in the graph (just an index)
    struct RGResourceHandle 
    {
        using IDType = uint16_t;
        static constexpr IDType InvalidID = std::numeric_limits<IDType>::max();

        IDType ID = InvalidID;

        bool IsValid() const { return ID != InvalidID; }
        bool operator==(const RGResourceHandle& other) const { return ID == other.ID; }
        bool operator!=(const RGResourceHandle& other) const { return ID != other.ID; }
    };
}