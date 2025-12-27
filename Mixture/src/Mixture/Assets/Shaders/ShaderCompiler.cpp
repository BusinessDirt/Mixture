#include "mxpch.hpp"
#include "Mixture/Assets/Shaders/ShaderCompiler.hpp"

#include "Mixture/Assets/AssetManager.hpp"

#include <dxc/dxcapi.h>
#include <dxc/WinAdapter.h>
#include <spirv_cross/spirv_msl.hpp>
#include <spirv_reflect.h>

namespace Mixture
{
    namespace
    {
        namespace Util
        {
            // Helper to convert SPIRV-Reflect formats to our clean enum
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

    Vector<uint8_t> ShaderCompiler::Compile(const std::string& source)
    {
        auto graphicsAPI = AssetManager::Get().GetGraphicsAPI();

        // These are initialized only ONCE the first time Compile is called.
        // C++11 guarantees this initialization is thread-safe.
        static CComPtr<IDxcUtils> pUtils;
        static CComPtr<IDxcCompiler3> pCompiler;
        static bool bInitialized = [&]() {
            DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&pUtils));
            DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&pCompiler));
            return true;
        }();

        // Create a blob from the source string
        CComPtr<IDxcBlobEncoding> pSource;
        pUtils->CreateBlob(source.c_str(), source.length(), CP_UTF8, &pSource);

        // Set up Compiler Arguments
        Vector<LPCWSTR> arguments;

        // Use Library profile (lib_6_x) to support multiple undefined entry points
        arguments.push_back(L"-T");
        arguments.push_back(L"lib_6_5"); // Shader Model 6.5

#ifdef OPAL_DEBUG
        arguments.push_back(L"-Od");
        arguments.push_back(L"-Zi");
        arguments.push_back(L"-Qembed_debug");
#else
        arguments.push_back(L"-O3");
#endif

        // API-Specific Flags
        if (graphicsAPI == RHI::GraphicsAPI::Vulkan || graphicsAPI == RHI::GraphicsAPI::Metal)
        {
            // Both Vulkan and Metal start with SPIR-V
            arguments.push_back(L"-spirv");
            arguments.push_back(L"-fspv-target-env=vulkan1.3");

            // Important: Preserve bindings so SPIRV-Cross can reflect them later
            arguments.push_back(L"-fvk-use-dx-layout");
        }

        // Run Compilation
        DxcBuffer sourceBuffer;
        sourceBuffer.Ptr = pSource->GetBufferPointer();
        sourceBuffer.Size = pSource->GetBufferSize();
        sourceBuffer.Encoding = DXC_CP_UTF8;

        CComPtr<IDxcResult> pResults;
        pCompiler->Compile(&sourceBuffer, arguments.data(), (uint32_t)arguments.size(), nullptr, IID_PPV_ARGS(&pResults));

        // Check for Errors
        CComPtr<IDxcBlobUtf8> pErrors;
        pResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrors), nullptr);
        if (pErrors && pErrors->GetStringLength() != 0)
        {
            OPAL_ERROR("Core/Assets", "Shader Compile Errror: {}", pErrors->GetStringPointer());
            return {};
        }

        // Get the Output Blob (DXIL or SPIR-V)
        CComPtr<IDxcBlob> pShaderBlob;
        pResults->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShaderBlob), nullptr);

        if (!pShaderBlob) return {};

        Vector<uint8_t> nativeBlob(pShaderBlob->GetBufferSize());
        memcpy(nativeBlob.data(), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize());
        return nativeBlob;
    }

    Vector<uint8_t> ShaderCompiler::ConvertToMSL(const Vector<uint8_t>& spv)
    {
        // Check if we have data
        if (spv.empty()) return {};

        // Cast the raw bytes (uint8) to SPIR-V words (uint32).
        // Note: This reinterpret_cast assumes 'spv.data()' is 4-byte aligned.
        // Standard vector allocators usually guarantee this, but be aware.
        const uint32_t* spirvData = reinterpret_cast<const uint32_t*>(spv.data());
        size_t wordCount = spv.size() / sizeof(uint32_t);

        // Initialize the Cross Compiler
        // (This parses the SPIR-V immediately)
        spirv_cross::CompilerMSL mslCompiler(spirvData, wordCount);

        // Set Options
        spirv_cross::CompilerMSL::Options mslOptions;
        mslOptions.set_msl_version(2, 3); // Metal 2.3 is a safe modern default
        mslOptions.argument_buffers_tier = spirv_cross::CompilerMSL::Options::ArgumentBuffersTier::Tier2;
        mslOptions.enable_decoration_binding = true;
        mslCompiler.set_msl_options(mslOptions);

        std::string mslSource;
        try
        {
            mslSource = mslCompiler.compile();
        }
        catch (const std::exception& e)
        {
            OPAL_ERROR("Core/Assets", "SPIRV-Cross compilation failed: {}", e.what());
            return {};
        }

        return Vector<uint8_t>(mslSource.begin(), mslSource.end());
    }

    ShaderReflectionData ShaderCompiler::ReflectSPIRV(const void* binaryData, size_t binarySize)
    {
        ShaderReflectionData data;
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
                attr.Format = Util::ConvertFormat(input->format);
                attr.Size = GetFormatStride(attr.Format);
                data.InputAttributes.push_back(attr);
            }
        }

        spvReflectDestroyShaderModule(&module);
        return data;
    }

    ShaderReflectionData ShaderCompiler::ReflectDXIL(const void* binaryData, size_t binarySize)
    {
        ShaderReflectionData data{};
        return data;
    }
}
