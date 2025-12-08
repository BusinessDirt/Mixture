#pragma once

#include "Mixture/Core/Base.hpp"

#include <cstdint>
#include <string>

namespace Mixture::RHI
{

    /**
     * @brief Defines how a buffer will be used in the rendering pipeline.
     */
    enum class BufferUsage : uint8_t 
    {
        /**
         * @brief Buffer containing vertex data.
         */
        Vertex,

        /**
         * @brief Buffer containing index data.
         */
        Index,

        /**
         * @brief Buffer containing uniform data (constants).
         */
        Uniform,

        /**
         * @brief Structured buffer (SSBO) for storage.
         */
        Storage,

        /**
         * @brief Source buffer for transfer operations.
         */
        TransferSrc,

        /**
         * @brief Destination buffer for transfer operations.
         */
        TransferDst
    };

    /**
     * @brief Descriptor structure used to create a buffer.
     */
    struct BufferDesc 
    {
        /**
         * @brief The size of the buffer in bytes.
         */
        uint64_t Size = 0;

        /**
         * @brief The usage flags for the buffer.
         */
        BufferUsage Usage = BufferUsage::Vertex;

        /**
         * @brief Debug name for the buffer.
         */
        const char* DebugName = "Unnamed Buffer";
    };

    /**
     * @brief Interface representing a GPU buffer.
     */
    class IBuffer 
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~IBuffer() = default;

        /**
         * @brief Retrieves the size of the buffer.
         * @return The size in bytes.
         */
        virtual uint64_t GetSize() const = 0;

        /**
         * @brief Retrieves the usage of the buffer.
         * @return The BufferUsage enum value.
         */
        virtual BufferUsage GetUsage() const = 0;
    };
}