#include "Jasper/Shaders/Types/DescriptorType.hpp"

namespace Jasper::Util
{
    const char* DescriptorTypeToString(const DescriptorType type)
    {
        switch (type)
        {
            case Sampler: return "Sampler";
            case CombinedImageSampler: return "Combined Image Sampler";
            case SampledImage: return "Sampled Image";
            case StorageItem: return "Storage Item";
            case UniformTexelBuffer: return "Uniform Texel Buffer";
            case StorageTexelBuffer: return "Storage Texel Buffer";
            case UniformBuffer: return "Uniform Buffer";
            case StorageBuffer: return "Storage Buffer";
            case UniformBufferDynamic: return "Uniform Buffer Dynamic";
            case StorageBufferDynamic: return "Storage Buffer Dynamic";
            case InputAttachment: return "Input Attachment";
            case InlineUniformBlock: return "Inline Uniform Block";
            case AccelerationStructureKHR: return "Acceleration Structure KHR";
            case AccelerationStructureNV: return "Acceleration Structure NV";
            case SampleWeightImageQCOM: return "Sample Weight Image QCOM";
            case BlockMatchImageQCOM: return "Block Match Image QCOM";
            case MutableEXT: return "Mutable EXT";
        }
        return "Unknown Descriptor Type";
    }
}
