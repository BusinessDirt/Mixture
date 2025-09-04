#include "Jasper/Shaders/Types/Format.hpp"
#include "Opal/Log.hpp"

#include <spirv_cross/spirv_common.hpp>

namespace Jasper::Util
{
    const char* FormatToString(const Format format)
    {
        switch (format)
        {
            case Undefined: return "Undefined";
            case R8_UNORM:
            case R8_USCALED:
            case R8_UINT:
                return "uint8_t";
            case R8_SNORM:
            case R8_SSCALED:
            case R8_SINT:
                return "int8_t";
            case R8G8_UNORM: 
            case R8G8_USCALED:
            case R8G8_UINT:
                return "uint8_t[2] (vec2)";
            case R8G8_SNORM:
            case R8G8_SSCALED:
            case R8G8_SINT:
                return "int8_t[2] (vec2)";
            case R8G8B8_UNORM:
            case B8G8R8_UNORM:
            case R8G8B8_USCALED:
            case B8G8R8_USCALED:
            case R8G8B8_UINT:
            case B8G8R8_UINT:
                return "uint8_t[3] (vec3)";
            case R8G8B8_SNORM:
            case B8G8R8_SNORM:
            case R8G8B8_SSCALED:
            case B8G8R8_SSCALED:
            case R8G8B8_SINT:
            case B8G8R8_SINT:
                return "int8_t[3] (vec3)";
            case R8G8B8A8_UNORM:
            case B8G8R8A8_UNORM:
            case R8G8B8A8_USCALED:
            case B8G8R8A8_USCALED:
            case R8G8B8A8_UINT:
            case B8G8R8A8_UINT:
                return "uint8_t[4] (vec4)";
            case R8G8B8A8_SNORM:
            case B8G8R8A8_SNORM:
            case R8G8B8A8_SSCALED:
            case B8G8R8A8_SSCALED:
            case R8G8B8A8_SINT:
            case B8G8R8A8_SINT:
                return "int8_t[4] (vec4)";
            case A8B8G8R8_UNORM_PACK32:
            case A2R10G10B10_UNORM_PACK32:
            case A2B10G10R10_UNORM_PACK32:
            case A8B8G8R8_USCALED_PACK32:
            case A2R10G10B10_USCALED_PACK32:
            case A2B10G10R10_USCALED_PACK32:
            case A8B8G8R8_UINT_PACK32:
            case A2R10G10B10_UINT_PACK32:
            case A2B10G10R10_UINT_PACK32:
                return "uint32_t (vec4)";
            case A8B8G8R8_SNORM_PACK32:
            case A2R10G10B10_SNORM_PACK32:
            case A2B10G10R10_SNORM_PACK32:
            case A8B8G8R8_SSCALED_PACK32:
            case A2R10G10B10_SSCALED_PACK32:
            case A2B10G10R10_SSCALED_PACK32:
            case A8B8G8R8_SINT_PACK32:
            case A2R10G10B10_SINT_PACK32:
            case A2B10G10R10_SINT_PACK32:
                return "int32_t (vec4)";
            case R16_UNORM:
            case R16_USCALED:
            case R16_UINT:
                return "uint16_t";
            case R16_SNORM:
            case R16_SSCALED:
            case R16_SINT:
                return "int16_t";
            case R16_SFLOAT:
                return "uint16_t (float)";
            case R16G16_UNORM:
            case R16G16_USCALED:
            case R16G16_UINT:
                return "uint16_t[2] (vec2)";
            case R16G16_SNORM:
            case R16G16_SSCALED:
            case R16G16_SINT:
                return "int16_t[2] (vec2)";
            case R16G16_SFLOAT:
                return "uint16_t[2] (float2)";
            case R16G16B16_UNORM:
            case R16G16B16_USCALED:
            case R16G16B16_UINT:
                return "uint16_t[3] (vec3)";
            case R16G16B16_SNORM:
            case R16G16B16_SSCALED:
            case R16G16B16_SINT:
                return "int16_t[3] (vec3)";
            case R16G16B16_SFLOAT:
                return "uint16_t[3] (float3)";
            case R16G16B16A16_UNORM:
            case R16G16B16A16_USCALED:
            case R16G16B16A16_UINT:
                return "uint16_t[4] (vec4)";
            case R16G16B16A16_SNORM:
            case R16G16B16A16_SSCALED:
            case R16G16B16A16_SINT:
                return "int16_t[4] (vec4)";
            case R16G16B16A16_SFLOAT: return "uint16_t[4] (float4)";
            case R32_UINT: return "uint32_t";
            case R32_SINT: return "int32_t";
            case R32_SFLOAT: return "float";
            case R32G32_UINT: return "uint32_t[2] (vec2)";
            case R32G32_SINT: return "int32_t[2] (vec2)";
            case R32G32_SFLOAT: return "float[2] (vec2)";
            case R32G32B32_UINT: return "uint32_t[3] (vec3)";
            case R32G32B32_SINT: return "int32_t[3] (vec3)";
            case R32G32B32_SFLOAT: return "float[3] (vec3)";
            case R32G32B32A32_UINT: return "uint32_t[4] (vec4)";
            case R32G32B32A32_SINT: return "int32_t[4] (vec4)";
            case R32G32B32A32_SFLOAT: return "float[4] (vec4)";
            case R64_UINT: return "uint64_t";
            case R64_SINT: return "int64_t";
            case R64_SFLOAT: return "double";
            case R64G64_UINT: return "uint64_t[2] (vec2)";
            case R64G64_SINT: return "int64_t[2] (vec2)";
            case R64G64_SFLOAT: return "double[2] (vec2)";
            case R64G64B64_UINT: return "uint64_t[3] (vec3)";
            case R64G64B64_SINT: return "int64_t[3] (vec3)";
            case R64G64B64_SFLOAT: return "double[3] (vec3)";
            case R64G64B64A64_UINT: return "uint64_t[4] (vec4)";
            case R64G64B64A64_SINT: return "int64_t[4] (vec4)";
            case R64G64B64A64_SFLOAT: return "double[4] (vec4)";
            case B10G11R11_UFLOAT_PACK32: return "uint32_t (vec3)";
        }
        return "Unknown Format";
    }

    Format GetFormatFromSPIRType(const spirv_cross::SPIRType& type)
    {
        using namespace spirv_cross;
        switch (type.basetype)
        {
            case SPIRType::Boolean:
                if (type.vecsize == 1) return R32_UINT;
                if (type.vecsize == 2) return R32G32_UINT;
                if (type.vecsize == 3) return R32G32B32_UINT;
                if (type.vecsize == 4) return R32G32B32A32_UINT;
                break;

            case SPIRType::SByte: // signed 8-bit
                if (type.vecsize == 1) return R8_SINT;
                if (type.vecsize == 2) return R8G8_SINT;
                if (type.vecsize == 3) return R8G8B8_SINT;
                if (type.vecsize == 4) return R8G8B8A8_SINT;
                break;

            case SPIRType::UByte: // unsigned 8-bit
                if (type.vecsize == 1) return R8_UINT;
                if (type.vecsize == 2) return R8G8_UINT;
                if (type.vecsize == 3) return R8G8B8_UINT;
                if (type.vecsize == 4) return R8G8B8A8_UINT;
                break;

            case SPIRType::Short: // signed 16-bit
                if (type.vecsize == 1) return R16_SINT;
                if (type.vecsize == 2) return R16G16_SINT;
                if (type.vecsize == 3) return R16G16B16_SINT;
                if (type.vecsize == 4) return R16G16B16A16_SINT;
                break;

            case SPIRType::UShort: // unsigned 16-bit
                if (type.vecsize == 1) return R16_UINT;
                if (type.vecsize == 2) return R16G16_UINT;
                if (type.vecsize == 3) return R16G16B16_UINT;
                if (type.vecsize == 4) return R16G16B16A16_UINT;
                break;

            case SPIRType::Int: // signed 32-bit
                if (type.vecsize == 1) return R32_SINT;
                if (type.vecsize == 2) return R32G32_SINT;
                if (type.vecsize == 3) return R32G32B32_SINT;
                if (type.vecsize == 4) return R32G32B32A32_SINT;
                break;

            case SPIRType::UInt: // unsigned 32-bit
                if (type.vecsize == 1) return R32_UINT;
                if (type.vecsize == 2) return R32G32_UINT;
                if (type.vecsize == 3) return R32G32B32_UINT;
                if (type.vecsize == 4) return R32G32B32A32_UINT;
                break;

            case SPIRType::Int64: // signed 64-bit (requires feature)
                if (type.vecsize == 1) return R64_SINT;
                if (type.vecsize == 2) return R64G64_SINT;
                if (type.vecsize == 3) return R64G64B64_SINT;
                if (type.vecsize == 4) return R64G64B64A64_SINT;
                break;

            case SPIRType::UInt64: // unsigned 64-bit (requires feature)
                if (type.vecsize == 1) return R64_UINT;
                if (type.vecsize == 2) return R64G64_UINT;
                if (type.vecsize == 3) return R64G64B64_UINT;
                if (type.vecsize == 4) return R64G64B64A64_UINT;
                break;

            case SPIRType::Half: // 16-bit float
                if (type.vecsize == 1) return R16_SFLOAT;
                if (type.vecsize == 2) return R16G16_SFLOAT;
                if (type.vecsize == 3) return R16G16B16_SFLOAT;
                if (type.vecsize == 4) return R16G16B16A16_SFLOAT;
                break;

            case SPIRType::Float: // 32-bit float
                if (type.vecsize == 1) return R32_SFLOAT;
                if (type.vecsize == 2) return R32G32_SFLOAT;
                if (type.vecsize == 3) return R32G32B32_SFLOAT;
                if (type.vecsize == 4) return R32G32B32A32_SFLOAT;
                break;

            case SPIRType::Double: // 64-bit float (requires feature)
                if (type.vecsize == 1) return R64_SFLOAT;
                if (type.vecsize == 2) return R64G64_SFLOAT;
                if (type.vecsize == 3) return R64G64B64_SFLOAT;
                if (type.vecsize == 4) return R64G64B64A64_SFLOAT;
                break;

            case SPIRType::Unknown:
            case SPIRType::Void:
            case SPIRType::AtomicCounter:
            case SPIRType::Struct:
            case SPIRType::Image:
            case SPIRType::SampledImage:
            case SPIRType::Sampler:
            case SPIRType::AccelerationStructure:
            case SPIRType::RayQuery:
            case SPIRType::ControlPointArray:
            case SPIRType::Interpolant:
            case SPIRType::Char:
                return Undefined;
        }

        OPAL_CORE_ERROR("Unsupported Format");
        return Undefined;
    }
}
