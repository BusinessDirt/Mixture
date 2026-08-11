#include "mxpch.hpp"
#include "Mixture/Assets/Shaders/ShaderCompiler.hpp"
#include "Mixture/Assets/Shaders/IShaderReflector.hpp"

#include "Mixture/Assets/AssetManager.hpp"

#ifdef _WIN32
    #include <Windows.h>
    #include <atlbase.h>
#else
    #include <dxc/WinAdapter.h>
#endif
#include <dxc/dxcapi.h>
#include <spirv_reflect.h>

namespace Mixture
{
    namespace
    {
        struct CompilerState
        {
            CComPtr<IDxcUtils> Utils;
            CComPtr<IDxcCompiler3> Compiler;
            bool Available = false;

            CompilerState()
            {
                const HRESULT utilsResult = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&Utils));
                const HRESULT compilerResult = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&Compiler));
                Available = SUCCEEDED(utilsResult) && SUCCEEDED(compilerResult) && Utils && Compiler;
            }
        };

        CompilerState& GetCompilerState()
        {
            static CompilerState state;
            return state;
        }

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

        class SPIRVShaderReflector : public IShaderReflector
        {
        public:
            ShaderReflectionData Reflect(const void* binaryData, size_t binarySize) const override;
        };

        // These remain distinct API seams even while DXC produces SPIR-V for
        // both backends. Slang can replace Metal's implementation independently.
        class VulkanShaderReflector final : public SPIRVShaderReflector {};
        class MetalShaderReflector final : public SPIRVShaderReflector {};

        class DXILShaderReflector final : public IShaderReflector
        {
        public:
            ShaderReflectionData Reflect(const void*, size_t) const override { return {}; }
        };
    }

    bool ShaderCompiler::IsAvailable()
    {
        return GetCompilerState().Available;
    }

    Vector<uint8_t> ShaderCompiler::Compile(const std::string& source)
    {
        return CompileDetailed(source).Bytecode;
    }

    Vector<uint8_t> ShaderCompiler::Compile(const std::string& source, RHI::GraphicsAPI graphicsAPI)
    {
        return CompileDetailed(source, graphicsAPI).Bytecode;
    }

    ShaderCompileResult ShaderCompiler::CompileDetailed(const std::string& source)
    {
        return CompileDetailed(source, AssetManager::Get().GetGraphicsAPI());
    }

    ShaderCompileResult ShaderCompiler::CompileDetailed(const std::string& source, RHI::GraphicsAPI graphicsAPI)
    {
        ShaderCompileResult result;
        if (source.size() > std::numeric_limits<uint32_t>::max())
        {
            result.Diagnostics = "Shader source exceeds the DXC input-size limit";
            return result;
        }
        CompilerState& state = GetCompilerState();
        if (!state.Available)
        {
            OPAL_ERROR("AssetManager", "DXC is unavailable; shader compilation cannot continue.");
            result.Diagnostics = "DXC is unavailable";
            return result;
        }

        // Create a blob from the source string
        CComPtr<IDxcBlobEncoding> pSource;
        const HRESULT blobResult = state.Utils->CreateBlob(
            source.c_str(), static_cast<uint32_t>(source.length()), CP_UTF8, &pSource);
        if (FAILED(blobResult) || !pSource)
        {
            result.Diagnostics = "DXC failed to create a source blob";
            return result;
        }

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
        const HRESULT compileResult = state.Compiler->Compile(&sourceBuffer, arguments.data(),
            static_cast<uint32_t>(arguments.size()), nullptr, IID_PPV_ARGS(&pResults));
        if (FAILED(compileResult) || !pResults)
        {
            result.Diagnostics = "DXC failed to start compilation";
            return result;
        }

        // Check for Errors
        CComPtr<IDxcBlobUtf8> pErrors;
        const HRESULT errorsResult = pResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrors), nullptr);
        if (FAILED(errorsResult))
        {
            result.Diagnostics = "DXC failed to retrieve compiler diagnostics";
            return result;
        }
        if (pErrors && pErrors->GetStringLength() != 0)
            result.Diagnostics.assign(pErrors->GetStringPointer(), pErrors->GetStringLength());

        HRESULT compilationStatus = E_FAIL;
        const HRESULT statusResult = pResults->GetStatus(&compilationStatus);
        if (FAILED(statusResult) || FAILED(compilationStatus))
        {
            if (!result.Diagnostics.empty())
                OPAL_ERROR("AssetManager", "Shader compile error: {}", result.Diagnostics);
            else
                result.Diagnostics = "DXC compilation failed without diagnostics";
            return result;
        }

        if (!result.Diagnostics.empty())
            OPAL_WARN("AssetManager", "Shader compile diagnostics: {}", result.Diagnostics);

        // Get the Output Blob (DXIL or SPIR-V)
        CComPtr<IDxcBlob> pShaderBlob;
        const HRESULT objectResult = pResults->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShaderBlob), nullptr);
        if (FAILED(objectResult) || !pShaderBlob || pShaderBlob->GetBufferSize() == 0)
        {
            result.Diagnostics = "DXC reported success but returned no shader object";
            return result;
        }

        result.Bytecode.resize(pShaderBlob->GetBufferSize());
        memcpy(result.Bytecode.data(), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize());
        return result;
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

    Scope<IShaderReflector> IShaderReflector::Create(RHI::GraphicsAPI graphicsAPI)
    {
        switch (graphicsAPI)
        {
            case RHI::GraphicsAPI::Vulkan:
            // DXC currently emits SPIR-V for Vulkan.
            return CreateScope<VulkanShaderReflector>();
            case RHI::GraphicsAPI::Metal:
            // The current compiler path also emits SPIR-V for Metal. Slang can
            // replace this with a Metal-specific reflector when it emits Metal bytecode.
            return CreateScope<MetalShaderReflector>();
            case RHI::GraphicsAPI::D3D12:
            return CreateScope<DXILShaderReflector>();
            case RHI::GraphicsAPI::None:
            return nullptr;
        }

        return nullptr;
    }
}
