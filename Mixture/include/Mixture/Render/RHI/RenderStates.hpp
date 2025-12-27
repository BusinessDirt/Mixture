#pragma once

/**
 * @file RenderStates.hpp
 * @brief Definitions for various render states (rasterizer, blend, depth/stencil).
 */

#include "Mixture/Core/Base.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <spdlog/fmt/fmt.h>

namespace Mixture::RHI
{

    /**
     * @brief Defines the basic geometric primitive type for drawing.
     */
    enum class PrimitiveTopology : uint8_t
    {
        /**
         * @brief Each set of three vertices defines a separate triangle.
         */
        TriangleList = 0,

        /**
         * @brief A series of connected triangles, where each new vertex forms a triangle with the two preceding vertices.
         */
        TriangleStrip,

        /**
         * @brief Each pair of vertices defines a separate line segment.
         */
        LineList,

        /**
         * @brief Each vertex defines a single point.
         */
        PointList
    };

    /**
     * @brief Specifies how polygons are rendered (filled, wireframe, or points).
     */
    enum class PolygonMode : uint8_t
    {
        /**
         * @brief Polygons are rendered as solid filled surfaces.
         */
        Fill = 0,

        /**
         * @brief Polygons are rendered as wireframes (lines connecting vertices).
         */
        Line,

        /**
         * @brief Polygons are rendered as points at each vertex.
         */
        Point
    };

    /**
     * @brief Determines which faces of a polygon are culled (discarded).
     */
    enum class CullMode : uint8_t
    {
        /**
         * @brief No culling is performed; both front and back faces are rendered.
         */
        None = 0,

        /**
         * @brief Front-facing polygons are culled.
         */
        Front,

        /**
         * @brief Back-facing polygons are culled.
         */
        Back
    };

    /**
     * @brief Defines the winding order of vertices that determines a front-facing primitive.
     */
    enum class FrontFace : uint8_t
    {
        /**
         * @brief Vertices ordered clockwise are considered front-facing.
         */
        Clockwise = 0,

        /**
         * @brief Vertices ordered counter-clockwise are considered front-facing.
         */
        CounterClockwise
    };

    /**
     * @brief Specifies the comparison operation used for depth and stencil tests.
     */
    enum class CompareOp : uint8_t
    {
        /**
         * @brief The test always fails.
         */
        Never = 0,

        /**
         * @brief The test passes if the new value is less than the existing value.
         */
        Less,

        /**
         * @brief The test passes if the new value is equal to the existing value.
         */
        Equal,

        /**
         * @brief The test passes if the new value is less than or equal to the existing value.
         */
        LessOrEqual,

        /**
         * @brief The test passes if the new value is greater than the existing value.
         */
        Greater,

        /**
         * @brief The test passes if the new value is not equal to the existing value.
         */
        NotEqual,

        /**
         * @brief The test passes if the new value is greater than or equal to the existing value.
         */
        GreaterOrEqual,

        /**
         * @brief The test always passes.
         */
        Always
    };

    /**
     * @brief Specifies the blend factor to be used in blending operations.
     */
    enum class BlendFactor : uint8_t
    {
        /**
         * @brief The blend factor is (0, 0, 0, 0).
         */
        Zero = 0,

        /**
         * @brief The blend factor is (1, 1, 1, 1).
         */
        One,

        /**
         * @brief The blend factor is the source color (Rs, Gs, Bs, As).
         */
        SrcColor,

        /**
         * @brief The blend factor is (1 - Rs, 1 - Gs, 1 - Bs, 1 - As).
         */
        InvSrcColor,

        /**
         * @brief The blend factor is (As, As, As, As).
         */
        SrcAlpha,

        /**
         * @brief The blend factor is (1 - As, 1 - As, 1 - As, 1 - As).
         */
        InvSrcAlpha,

        /**
         * @brief The blend factor is (Ad, Ad, Ad, Ad).
         */
        DstAlpha,

        /**
         * @brief The blend factor is (1 - Ad, 1 - Ad, 1 - Ad, 1 - Ad).
         */
        InvDstAlpha,

        /**
         * @brief The blend factor is the destination color (Rd, Gd, Bd, Ad).
         */
        DstColor,

        /**
         * @brief The blend factor is (1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad).
         */
        InvDstColor,

        /**
         * @brief The blend factor is (min(As, 1 - Ad), min(As, 1 - Ad), min(As, 1 - Ad), 1).
         */
        SrcAlphaSaturate
    };

    /**
     * @brief Specifies the blending operation to be performed.
     */
    enum class BlendOp : uint8_t
    {
        /**
         * @brief Add source and destination components: `Src * Factor + Dst * Factor`.
         */
        Add = 0,

        /**
         * @brief Subtract destination from source: `Src * Factor - Dst * Factor`.
         */
        Subtract,

        /**
         * @brief Subtract source from destination: `Dst * Factor - Src * Factor`.
         */
        ReverseSubtract,

        /**
         * @brief Take the minimum of source and destination components.
         */
        Min,

        /**
         * @brief Take the maximum of source and destination components.
         */
        Max
    };

    /**
     * @brief Specifies the loading operation to be performed.
     */
    enum class LoadOp : uint8_t
    {
        /**
         * @brief The existing contents of the attachment will be loaded.
         */
        Load = 0,

        /**
         * @brief The attachment will be cleared to a clear color/depth value.
         */
        Clear,

        /**
         * @brief The contents of the attachment prior to rendering are undefined.
         */
        DontCare,

        /**
         * @brief No load operation specified.
         */
        None
    };

    /**
     * @brief Specifies the storing operation to be performed.
     */
    enum class StoreOp : uint8_t
    {
        /**
         * @brief The rendered contents of the attachment will be stored.
         */
        Store = 0,

        /**
         * @brief The contents of the attachment after rendering are undefined.
         */
        DontCare,

        /**
         * @brief No store operation specified.
         */
        None,
    };

    /**
     * @brief Specifies which color components are written to the render target.
     */
    enum class ColorWriteMask : uint8_t
    {
        /**
         * @brief Enable writing to the red component.
         */
        Red   = 1 << 0,

        /**
         * @brief Enable writing to the green component.
         */
        Green = 1 << 1,

        /**
         * @brief Enable writing to the blue component.
         */
        Blue  = 1 << 2,

        /**
         * @brief Enable writing to the alpha component.
         */
        Alpha = 1 << 3,

        /**
         * @brief Enable writing to all color components (Red, Green, Blue, Alpha).
         */
        All   = 0xF
    };

    /**
     * @brief Defines the state for the rasterizer stage of the graphics pipeline.
     */
    struct RasterizerState
    {
        /**
         * @brief The polygon rendering mode (fill, line, or point). Defaults to `PolygonMode::Fill`.
         */
        PolygonMode polygonMode = PolygonMode::Fill;

        /**
         * @brief The face culling mode (none, front, or back). Defaults to `CullMode::Back`.
         */
        CullMode cullMode = CullMode::Back;

        /**
         * @brief The winding order that defines front-facing primitives. Defaults to `FrontFace::CounterClockwise`.
         */
        FrontFace frontFace = FrontFace::CounterClockwise;

        /**
         * @brief The width of lines when `polygonMode` is `Line`. Defaults to `1.0f`.
         */
        float lineWidth = 1.0f;

        bool operator==(const RasterizerState& other) const
        {
            return polygonMode == other.polygonMode &&
                   cullMode == other.cullMode &&
                   frontFace == other.frontFace &&
                   lineWidth == other.lineWidth;
        }
    };

    /**
     * @brief Defines the state for depth and stencil testing.
     */
    struct DepthStencilState
    {
        /**
         * @brief Enables or disables depth testing. Defaults to `true`.
         */
        bool depthTest = true;

        /**
         * @brief Enables or disables writing to the depth buffer. Defaults to `true`.
         */
        bool depthWrite = true;

        /**
         * @brief The comparison operation used for depth testing. Defaults to `CompareOp::Less`.
         */
        CompareOp depthCompareOp = CompareOp::Less;

        bool operator==(const DepthStencilState& other) const
        {
            return depthTest == other.depthTest &&
                   depthWrite == other.depthWrite &&
                   depthCompareOp == other.depthCompareOp;
        }
    };

    /**
     * @brief Defines the state for blending operations.
     */
    struct BlendState
    {
        /**
         * @brief Enables or disables blending for the render target. Defaults to `false`.
         */
        bool enabled = false;

        /**
         * @brief The blend factor for the source color components. Defaults to `BlendFactor::SrcAlpha`.
         */
        BlendFactor srcColor = BlendFactor::SrcAlpha;

        /**
         * @brief The blend factor for the destination color components. Defaults to `BlendFactor::InvSrcAlpha`.
         */
        BlendFactor dstColor = BlendFactor::InvSrcAlpha;

        /**
         * @brief The blending operation for color components. Defaults to `BlendOp::Add`.
         */
        BlendOp colorOp = BlendOp::Add;

        /**
         * @brief The blend factor for the source alpha component. Defaults to `BlendFactor::One`.
         */
        BlendFactor srcAlpha = BlendFactor::One;

        /**
         * @brief The blend factor for the destination alpha component. Defaults to `BlendFactor::Zero`.
         */
        BlendFactor dstAlpha = BlendFactor::Zero;

        /**
         * @brief The blending operation for the alpha component. Defaults to `BlendOp::Add`.
         */
        BlendOp alphaOp = BlendOp::Add;

        bool operator==(const BlendState& other) const
        {
            return enabled == other.enabled &&
                   srcColor == other.srcColor &&
                   dstColor == other.dstColor &&
                   colorOp == other.colorOp &&
                   srcAlpha == other.srcAlpha &&
                   dstAlpha == other.dstAlpha &&
                   alphaOp == other.alphaOp;
        }
    };

    inline std::string_view ToString(PrimitiveTopology topology)
    {
        switch (topology)
        {
            case PrimitiveTopology::TriangleList: return "TriangleList";
            case PrimitiveTopology::TriangleStrip: return "TriangleStrip";
            case PrimitiveTopology::LineList: return "LineList";
            case PrimitiveTopology::PointList: return "PointList";
            default: return "Unknown";
        }
    }

    inline std::string_view ToString(PolygonMode mode)
    {
        switch (mode)
        {
            case PolygonMode::Fill: return "Fill";
            case PolygonMode::Line: return "Line";
            case PolygonMode::Point: return "Point";
            default: return "Unknown";
        }
    }

    inline std::string_view ToString(CullMode mode)
    {
        switch (mode)
        {
            case CullMode::None: return "None";
            case CullMode::Front: return "Front";
            case CullMode::Back: return "Back";
            default: return "Unknown";
        }
    }

    inline std::string_view ToString(FrontFace face)
    {
        switch (face)
        {
            case FrontFace::Clockwise: return "Clockwise";
            case FrontFace::CounterClockwise: return "CounterClockwise";
            default: return "Unknown";
        }
    }

    inline std::string_view ToString(CompareOp op)
    {
        switch (op)
        {
            case CompareOp::Never: return "Never";
            case CompareOp::Less: return "Less";
            case CompareOp::Equal: return "Equal";
            case CompareOp::LessOrEqual: return "LessOrEqual";
            case CompareOp::Greater: return "Greater";
            case CompareOp::NotEqual: return "NotEqual";
            case CompareOp::GreaterOrEqual: return "GreaterOrEqual";
            case CompareOp::Always: return "Always";
            default: return "Unknown";
        }
    }

    inline std::string_view ToString(BlendFactor factor)
    {
        switch (factor)
        {
            case BlendFactor::Zero: return "Zero";
            case BlendFactor::One: return "One";
            case BlendFactor::SrcColor: return "SrcColor";
            case BlendFactor::InvSrcColor: return "InvSrcColor";
            case BlendFactor::SrcAlpha: return "SrcAlpha";
            case BlendFactor::InvSrcAlpha: return "InvSrcAlpha";
            case BlendFactor::DstAlpha: return "DstAlpha";
            case BlendFactor::InvDstAlpha: return "InvDstAlpha";
            case BlendFactor::DstColor: return "DstColor";
            case BlendFactor::InvDstColor: return "InvDstColor";
            case BlendFactor::SrcAlphaSaturate: return "SrcAlphaSaturate";
            default: return "Unknown";
        }
    }

    inline std::string_view ToString(BlendOp op)
    {
        switch (op)
        {
            case BlendOp::Add: return "Add";
            case BlendOp::Subtract: return "Subtract";
            case BlendOp::ReverseSubtract: return "ReverseSubtract";
            case BlendOp::Min: return "Min";
            case BlendOp::Max: return "Max";
            default: return "Unknown";
        }
    }

    inline std::string_view ToString(LoadOp op)
    {
        switch (op)
        {
            case LoadOp::Load: return "Load";
            case LoadOp::Clear: return "Clear";
            case LoadOp::DontCare: return "DontCare";
            case LoadOp::None: return "None";
            default: return "Unknown";
        }
    }

    inline std::string_view ToString(StoreOp op)
    {
        switch (op)
        {
            case StoreOp::Store: return "Store";
            case StoreOp::DontCare: return "DontCare";
            case StoreOp::None: return "None";
            default: return "Unknown";
        }
    }

    inline std::string ToString(ColorWriteMask mask)
    {
        if (mask == ColorWriteMask::All) return "All";
        if (static_cast<uint8_t>(mask) == 0) return "None";
        std::string result;
        if ((static_cast<uint8_t>(mask) & static_cast<uint8_t>(ColorWriteMask::Red)) != 0) result += "R";
        if ((static_cast<uint8_t>(mask) & static_cast<uint8_t>(ColorWriteMask::Green)) != 0) result += "G";
        if ((static_cast<uint8_t>(mask) & static_cast<uint8_t>(ColorWriteMask::Blue)) != 0) result += "B";
        if ((static_cast<uint8_t>(mask) & static_cast<uint8_t>(ColorWriteMask::Alpha)) != 0) result += "A";
        return result;
    }

    inline std::string ToString(const RasterizerState& state)
    {
        return fmt::format("RasterizerState{{ PolygonMode: {}, CullMode: {}, FrontFace: {}, LineWidth: {} }}",
            ToString(state.polygonMode), ToString(state.cullMode), ToString(state.frontFace), state.lineWidth);
    }

    inline std::string ToString(const DepthStencilState& state)
    {
        return fmt::format("DepthStencilState{{ DepthTest: {}, DepthWrite: {}, CompareOp: {} }}",
            state.depthTest, state.depthWrite, ToString(state.depthCompareOp));
    }

    inline std::string ToString(const BlendState& state)
    {
        if (!state.enabled) return "BlendState{ Disabled }";
        return fmt::format("BlendState{{ SrcColor: {}, DstColor: {}, ColorOp: {}, SrcAlpha: {}, DstAlpha: {}, AlphaOp: {} }}",
            ToString(state.srcColor), ToString(state.dstColor), ToString(state.colorOp),
            ToString(state.srcAlpha), ToString(state.dstAlpha), ToString(state.alphaOp));
    }
}
