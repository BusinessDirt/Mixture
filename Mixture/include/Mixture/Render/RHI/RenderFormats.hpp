#pragma once

#include "Mixture/Core/Base.hpp"

#include <cstdint>

namespace Mixture::RHI 
{
    /**
     * @enum Format
     * @brief Defines various data formats for render resources like textures and buffers.
     *
     * This enum provides a comprehensive list of formats, categorized by bit depth,
     * type (UNORM, SFLOAT, INT, UINT), and intended use (e.g., Albedo, HDR, Depth).
     * These formats are crucial for specifying how data is interpreted by the GPU.
     */
    enum class Format : uint32_t 
    {
        /** @brief Default undefined format. */
        Undefined = 0,

        /**
         * @brief 8-bit single channel unsigned normalized format.
         * Range: [0.0, 1.0]. Used for single-channel textures or masks.
         */
        R8_UNORM,

        /**
         * @brief 8-bit dual channel unsigned normalized format.
         * Range: [0.0, 1.0].
         */
        R8G8_UNORM,

        /**
         * @brief 8-bit triple channel unsigned normalized format.
         * Range: [0.0, 1.0].
         */
        R8G8B8_UNORM,

        /**
         * @brief 8-bit four channel unsigned normalized format (RGBA).
         * Range: [0.0, 1.0]. Common for color textures.
         */
        R8G8B8A8_UNORM,

        /**
         * @brief 8-bit four channel unsigned normalized format (BGRA).
         * Range: [0.0, 1.0]. Standard format for swapchains on some platforms.
         */
        B8G8R8A8_UNORM,

        /**
         * @brief 16-bit single channel floating-point format.
         * Range: [-65504, 65504]. Used for HDR rendering, bloom, or single-channel data requiring higher precision.
         */
        R16_FLOAT,

        /**
         * @brief 16-bit dual channel floating-point format.
         * Range: [-65504, 65504].
         */
        R16G16_FLOAT,

        /**
         * @brief 16-bit triple channel floating-point format.
         * Range: [-65504, 65504].
         */
        R16G16B16_FLOAT,

        /**
         * @brief 16-bit four channel floating-point format.
         * Range: [-65504, 65504]. Used for HDR rendering, bloom.
         */
        R16G16B16A16_FLOAT,

        /**
         * @brief 32-bit single channel floating-point format.
         * High precision. Used for position data, world space normals, or other single-channel high-precision data.
         */
        R32_FLOAT,

        /**
         * @brief 32-bit dual channel floating-point format.
         * High precision.
         */
        R32G32_FLOAT,

        /**
         * @brief 32-bit triple channel floating-point format.
         * High precision.
         */
        R32G32B32_FLOAT,

        /**
         * @brief 32-bit four channel floating-point format.
         * High precision. Used for Position data, World Space Normals.
         */
        R32G32B32A32_FLOAT,
        
        /**
         * @brief 32-bit single channel signed integer format.
         * Used for IDs (Entity ID buffers), atomic counters, or other integer data.
         */
        R32_INT,

        /**
         * @brief 32-bit single channel unsigned integer format.
         * Used for IDs (Entity ID buffers), atomic counters, or other integer data.
         */
        R32_UINT,

        /**
         * @brief 32-bit single channel floating-point depth format.
         * Best for modern PC (no stencil).
         */
        D32_FLOAT,

        /**
         * @brief 24-bit unsigned normalized depth with 8-bit unsigned integer stencil format.
         * Common on older hardware/consoles.
         */
        D24_UNORM_S8_UINT,

        /**
         * @brief 32-bit floating-point depth with 8-bit unsigned integer stencil format.
         * High precision + Stencil.
         */
        D32_FLOAT_S8_UINT
    };

    /**
     * @brief Calculates the stride (size in bytes) of a given Format.
     *
     * This helper function is essential for setting up vertex buffers and other
     * data structures where the byte size of each element needs to be known.
     *
     * @param format The Format enum value for which to determine the stride.
     * @return The size in bytes of a single element of the specified format.
     *         Returns 0 for `Undefined` format or unrecognized formats.
     */
    inline uint32_t GetFormatStride(Format format) 
    {
        switch (format) 
        {
            case Format::R8_UNORM:           return 1;
            case Format::R8G8_UNORM:         return 2;
            case Format::R8G8B8_UNORM:       return 3;
            case Format::R8G8B8A8_UNORM:     return 4;
            case Format::B8G8R8A8_UNORM:     return 4;

            case Format::R16_FLOAT:          return 2;
            case Format::R16G16_FLOAT:       return 4;
            case Format::R16G16B16_FLOAT:    return 6;
            case Format::R16G16B16A16_FLOAT: return 8;

            case Format::R32_FLOAT:          return 4;
            case Format::R32G32_FLOAT:       return 8;
            case Format::R32G32B32_FLOAT:    return 12;
            case Format::R32G32B32A32_FLOAT: return 16;
            
            case Format::R32_INT:            return 4;
            case Format::R32_UINT:           return 4;

            case Format::D32_FLOAT:          return 4;
            // D24_S8 is packed into 32 bits usually
            case Format::D24_UNORM_S8_UINT:  return 4; 
            // D32_S8 is often 64 bits (32 depth + 8 stencil + 24 padding)
            case Format::D32_FLOAT_S8_UINT:  return 8; 

            default: return 0;
        }
    }
}