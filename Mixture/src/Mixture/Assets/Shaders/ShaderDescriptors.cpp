#include "mxpch.hpp"
#include "ShaderDescriptors.hpp"

namespace Mixture
{
    std::string DescriptorTypeToString(DescriptorType type)
    {
        switch (type)
        {
            STRINGIFY_CASE(DESCRIPTOR_TYPE_SAMPLER);
            STRINGIFY_CASE(DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            STRINGIFY_CASE(DESCRIPTOR_TYPE_SAMPLED_IMAGE);
            STRINGIFY_CASE(DESCRIPTOR_TYPE_STORAGE_IMAGE);
            STRINGIFY_CASE(DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER);
            STRINGIFY_CASE(DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER);
            STRINGIFY_CASE(DESCRIPTOR_TYPE_UNIFORM_BUFFER);
            STRINGIFY_CASE(DESCRIPTOR_TYPE_STORAGE_BUFFER);
            STRINGIFY_CASE(DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);
            STRINGIFY_CASE(DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC);
            STRINGIFY_CASE(DESCRIPTOR_TYPE_INPUT_ATTACHMENT);
            STRINGIFY_CASE(DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK);
            STRINGIFY_CASE(DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR);
            STRINGIFY_CASE(DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV);
            STRINGIFY_CASE(DESCRIPTOR_TYPE_SAMPLE_WEIGHT_IMAGE_QCOM);
            STRINGIFY_CASE(DESCRIPTOR_TYPE_BLOCK_MATCH_IMAGE_QCOM);
            STRINGIFY_CASE(DESCRIPTOR_TYPE_MUTABLE_EXT);
            STRINGIFY_CASE(DESCRIPTOR_TYPE_MAX_ENUM);

        default: return "UNKNOWN_DESCRIPTOR_TYPE";
        }
    }

    Vector<DescriptorLayout> GetDefaultLayouts()
    {
        DescriptorLayout globalLayout{};
        globalLayout.Set = 0;
        globalLayout.Elements[0] = DescriptorBinding{ DESCRIPTOR_TYPE_UNIFORM_BUFFER, sizeof(GlobalUniformBuffer) };

        DescriptorLayout instanceLayout{};
        instanceLayout.Set = 1;
        instanceLayout.Elements[0] = DescriptorBinding{ DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0 };

        return Vector<DescriptorLayout>{ globalLayout, instanceLayout };
    }
}