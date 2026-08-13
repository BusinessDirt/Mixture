#pragma once
#if defined(OPAL_PLATFORM_DARWIN)

/**
 * @file Buffer.hpp
 * @brief Metal implementation of the Buffer interface.
 */

#include "Platform/Metal/Definitions.hpp"
#include "Mixture/Render/RHI/IBuffer.hpp"

#include <span>

namespace Mixture::Metal
{
    class Device;

    /**
     * @brief Metal implementation of a GPU buffer.
     */
    class Buffer : public RHI::IBuffer
    {
    public:
        Buffer(Ref<Device> device, const RHI::BufferDesc& desc, std::span<const std::byte> initialData = {});
        ~Buffer();

        uint64_t GetSize() const override { return m_Desc.Size; }
        RHI::BufferUsage GetUsage() const override { return m_Desc.Usage; }

        MTL::Buffer* GetHandle() const { return m_Buffer; }

    private:
        Ref<Device> m_Device;
        RHI::BufferDesc m_Desc;
        MTL::Buffer* m_Buffer = nullptr;
    };
}

#endif
