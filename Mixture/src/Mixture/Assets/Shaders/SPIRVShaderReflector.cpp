#include "mxpch.hpp"
#include "Mixture/Assets/Shaders/SPIRVShaderReflector.hpp"

namespace Mixture
{
    namespace
    {
        namespace Util
        {
            RHI::Format ConvertFormat(SpvReflectFormat fmt)
            {
                switch (fmt) {
                    case SPV_REFLECT_FORMAT_R32_SFLOAT:          return RHI::Format::R32_FLOAT;
                    case SPV_REFLECT_FORMAT_R32G32_SFLOAT:       return RHI::Format::R32G32_FLOAT;
                    case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:    return RHI::Format::R32G32B32_FLOAT;
                    case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT: return RHI::Format::R32G32B32A32_FLOAT;
                    case SPV_REFLECT_FORMAT_R32_SINT:            return RHI::Format::R32_INT;
                    case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:   return RHI::Format::R32G32B32A32_INT;
                    default:                                     return RHI::Format::Undefined;
                }
            }
        }
    }


    ShaderReflectionData SPIRVShaderReflector::Reflect(const void* binaryData, size_t binarySize) const
    {
        ShaderReflectionData data;
        if (!binaryData || binarySize < sizeof(uint32_t) || binarySize % sizeof(uint32_t) != 0
            || reinterpret_cast<uintptr_t>(binaryData) % alignof(uint32_t) != 0)
            return data;

        uint32_t magic = 0;
        std::memcpy(&magic, binaryData, sizeof(magic));
        if (magic != 0x07230203u) return data;

        SpvReflectShaderModule module;

        if (spvReflectCreateShaderModule(binarySize, binaryData, &module) != SPV_REFLECT_RESULT_SUCCESS)
        {
            return data;
        }

        // Iterate over all entry points found in the SPIR-V
        for (uint32_t i = 0; i < module.entry_point_count; i++)
        {
            const SpvReflectEntryPoint* entry = &module.entry_points[i];
            std::string name = entry->name;

            // Map SPIR-V stage bit to your Engine's RHI Stage
            if (entry->shader_stage == SPV_REFLECT_SHADER_STAGE_VERTEX_BIT)
            {
                data.EntryPoints[RHI::ShaderStage::Vertex] = name;
            }
            else if (entry->shader_stage == SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT)
            {
                data.EntryPoints[RHI::ShaderStage::Fragment] = name;
            }
            else if (entry->shader_stage == SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT)
            {
                data.EntryPoints[RHI::ShaderStage::Compute] = name;
            }
        }

        // -------------------------------------------------------------------------
        // REFLECT RESOURCES (Uniforms, Textures, Samplers)
        // -------------------------------------------------------------------------
        uint32_t count = 0;
        spvReflectEnumerateDescriptorSets(&module, &count, nullptr);
        std::vector<SpvReflectDescriptorSet*> sets(count);
        spvReflectEnumerateDescriptorSets(&module, &count, sets.data());

        for (const auto* set : sets)
        {
            for (uint32_t i = 0; i < set->binding_count; i++)
            {
                const auto* binding = set->bindings[i];

                ShaderReflectionData::Resource res = {};
                res.Name = binding->name ? binding->name : "";
                res.Set = binding->set;
                res.Binding = binding->binding;
                res.Count = binding->count;
                res.Size = 0;

                switch (binding->descriptor_type)
                {
                    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                        res.Type = ShaderReflectionData::ResourceType::UniformBuffer;
                        res.Size = binding->block.size;
                        data.UniformBuffers.push_back(res);
                        break;

                    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                        res.Type = ShaderReflectionData::ResourceType::StorageBuffer;
                        res.Size = binding->block.size;
                        data.StorageBuffers.push_back(res);
                        break;

                    case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                    case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                        res.Type = ShaderReflectionData::ResourceType::SampledImage;
                        data.Textures.push_back(res);
                        break;

                    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                        res.Type = ShaderReflectionData::ResourceType::StorageImage;
                        data.StorageImages.push_back(res);
                        break;

                    case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
                        res.Type = ShaderReflectionData::ResourceType::Sampler;
                        data.Samplers.push_back(res);
                        break;
                    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
                    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
                    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
                    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
                    case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
                    case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
                        break;
                }
            }
        }

        // -------------------------------------------------------------------------
        // REFLECT PUSH CONSTANTS
        // -------------------------------------------------------------------------
        count = 0;
        spvReflectEnumeratePushConstantBlocks(&module, &count, nullptr);
        std::vector<SpvReflectBlockVariable*> pcs(count);
        spvReflectEnumeratePushConstantBlocks(&module, &count, pcs.data());

        for (const auto* pc : pcs)
        {
            ShaderReflectionData::PushConstant outPC;
            outPC.Name = pc->name ? pc->name : "PushConstant";
            outPC.Offset = pc->offset;
            outPC.Size = pc->size;
            outPC.ShaderStage = module.shader_stage;
            data.PushConstants.push_back(outPC);
        }

        // -------------------------------------------------------------------------
        // REFLECT VERTEX ATTRIBUTES
        // -------------------------------------------------------------------------
        if (module.shader_stage == SPV_REFLECT_SHADER_STAGE_VERTEX_BIT)
        {
            count = 0;
            spvReflectEnumerateInputVariables(&module, &count, nullptr);
            std::vector<SpvReflectInterfaceVariable*> inputs(count);
            spvReflectEnumerateInputVariables(&module, &count, inputs.data());

            for (const auto* input : inputs)
            {
                // Skip built-in variables (gl_VertexIndex, gl_InstanceIndex)
                if (input->built_in != -1) continue;
                if (input->name && strncmp(input->name, "gl_", 3) == 0) continue;

                ShaderReflectionData::VertexAttribute attr;
                attr.Name = input->name ? input->name : "";
                attr.Location = input->location;
                attr.PixelFormat = Util::ConvertFormat(input->format);
                attr.Size = GetFormatStride(attr.PixelFormat);
                data.InputAttributes.push_back(attr);
            }
        }

        spvReflectDestroyShaderModule(&module);
        return data;
    }
}
