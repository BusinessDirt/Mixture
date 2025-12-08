#pragma once

#include "Mixture/Core/Base.hpp"

#include <cstdint>

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
         * @brief
         */
        Load = 0,

        /**
         * @brief
         */
        Clear,

        /**
         * @brief
         */
        DontCare,

        /**
         * @brief
         */
        None
    };

    /**
     * @brief Specifies the storing operation to be performed.
     */
    enum class StoreOp : uint8_t
    {
        /**
         * @brief
         */
        Store = 0,

        /**
         * @brief
         */
        DontCare,

        /**
         * @brief
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
    };
}