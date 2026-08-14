#include "mxpch.hpp"
#include "Mixture/Assets/Shaders/SlangShaderReflector.hpp"

namespace Mixture
{
    namespace
    {
        struct VertexElementType
        {
            slang::TypeReflection* ScalarType = nullptr;
            uint32_t ComponentCount = 0;
        };

        std::optional<VertexElementType> GetVertexElementType(slang::TypeReflection& type)
        {
            switch (type.getKind())
            {
                case slang::TypeReflection::Kind::Scalar:
                    return VertexElementType{ &type, 1 };

                case slang::TypeReflection::Kind::Vector:
                {
                    slang::TypeReflection* elementType = type.getElementType();
                    const uint32_t componentCount = static_cast<uint32_t>(type.getElementCount());
                    if (!elementType || elementType->getKind() != slang::TypeReflection::Kind::Scalar
                        || componentCount == 0 || componentCount > 4)
                    {
                        return std::nullopt;
                    }
                    return VertexElementType{ elementType, componentCount };
                }

                // Arrays, matrices, structs, and resources require more than one
                // vertex attribute or a dedicated binding convention. They cannot
                // be represented by the current single-format VertexAttribute.
                default:
                    return std::nullopt;
            }
        }

        RHI::Format GetFloatFormat(uint32_t components, bool halfPrecision)
        {
            if (halfPrecision)
            {
                switch (components)
                {
                    case 1: return RHI::Format::R16_FLOAT;
                    case 2: return RHI::Format::R16G16_FLOAT;
                    case 3: return RHI::Format::R16G16B16_FLOAT;
                    case 4: return RHI::Format::R16G16B16A16_FLOAT;
                    default: return RHI::Format::Undefined;
                }
            }

            switch (components)
            {
                case 1: return RHI::Format::R32_FLOAT;
                case 2: return RHI::Format::R32G32_FLOAT;
                case 3: return RHI::Format::R32G32B32_FLOAT;
                case 4: return RHI::Format::R32G32B32A32_FLOAT;
                default: return RHI::Format::Undefined;
            }
        }
    }

    ShaderReflectionData SlangShaderReflector::Reflect(slang::ProgramLayout* layout) const
    {
        ShaderReflectionData data;
        if (!layout) return data;

        ReflectEntryPoints(*layout, data);
        ReflectResources(*layout, data);
        ReflectVertexInputs(*layout, data);

        return data;
    }

    void SlangShaderReflector::ReflectEntryPoints(slang::ProgramLayout& layout, ShaderReflectionData& data)
    {
        for (unsigned int index = 0; index < layout.getEntryPointCount(); ++index)
        {
            slang::EntryPointReflection* entryPoint = layout.getEntryPointByIndex(index);
            const RHI::ShaderStage stage = ToShaderStage(entryPoint->getStage());
            if (stage != RHI::ShaderStage::Compute || entryPoint->getStage() == SLANG_STAGE_COMPUTE)
                data.EntryPoints[stage] = entryPoint->getName();
        }
    }

    void SlangShaderReflector::ReflectResources(slang::ProgramLayout& layout, ShaderReflectionData& data)
    {
        for (unsigned int index = 0; index < layout.getParameterCount(); ++index)
        {
            slang::VariableLayoutReflection* variable = layout.getParameterByIndex(index);
            std::string variableName = variable->getName() ? variable->getName() : "";

            slang::ParameterCategory category = variable->getCategory();
            if (category == slang::ParameterCategory::PushConstantBuffer || variableName == "pushConstants")
            {
                AddPushConstant(layout, *variable, data);
            }
            else
            {
                AddResource(*variable, data);
            }
        }
    }

    void SlangShaderReflector::ReflectVertexInputs(slang::ProgramLayout& layout, ShaderReflectionData& data)
    {
        for (unsigned int entryIndex = 0; entryIndex < layout.getEntryPointCount(); ++entryIndex)
        {
            slang::EntryPointReflection* entryPoint = layout.getEntryPointByIndex(entryIndex);
            if (entryPoint->getStage() != SLANG_STAGE_VERTEX) continue;

            for (unsigned int inputIndex = 0; inputIndex < entryPoint->getParameterCount(); ++inputIndex)
            {
                slang::VariableLayoutReflection* input = entryPoint->getParameterByIndex(inputIndex);
                if (input->getCategory() != slang::ParameterCategory::VaryingInput) continue;
                if (input->getSemanticName() && std::string_view(input->getSemanticName()) == "SV_VertexID") continue;
                AddVertexInput(*input, data);
            }
        }
    }

    void SlangShaderReflector::AddResource(slang::VariableLayoutReflection& variable, ShaderReflectionData& data)
    {
        slang::TypeReflection* type = variable.getType();
        slang::TypeLayoutReflection* typeLayout = variable.getTypeLayout();
        if (!type || !typeLayout) return;

        ShaderReflectionData::Resource resource{};
        resource.Name = variable.getName() ? variable.getName() : "";
        resource.Binding = variable.getBindingIndex();
        resource.Set = variable.getBindingSpace();
        resource.Count = std::max(1u, static_cast<uint32_t>(type->getElementCount()));

        if (type->getKind() == slang::TypeReflection::Kind::ConstantBuffer)
        {
            resource.Type = ShaderReflectionData::ResourceType::UniformBuffer;

            if (auto* elementLayout = typeLayout->getElementTypeLayout())
                resource.Size = static_cast<uint32_t>(elementLayout->getSize());

            data.UniformBuffers.push_back(std::move(resource));
        }
        else if (type->getKind() == slang::TypeReflection::Kind::TextureBuffer)
        {
            if (type->getResourceAccess() == SLANG_RESOURCE_ACCESS_READ_WRITE)
            {
                resource.Type = ShaderReflectionData::ResourceType::StorageImage;
                data.StorageImages.push_back(std::move(resource));
            }
            else
            {
                resource.Type = ShaderReflectionData::ResourceType::SampledImage;
                data.Textures.push_back(std::move(resource));
            }
        }
        else if (type->getKind() == slang::TypeReflection::Kind::SamplerState)
        {
            resource.Type = ShaderReflectionData::ResourceType::Sampler;
            data.Samplers.push_back(std::move(resource));
        }
        else if (type->getKind() == slang::TypeReflection::Kind::Resource)
        {
            resource.Type = ShaderReflectionData::ResourceType::StorageBuffer;
            if (auto* elementLayout = typeLayout->getElementTypeLayout()) resource.Size = static_cast<uint32_t>(elementLayout->getSize());
            data.StorageBuffers.push_back(std::move(resource));
        }
    }

    void SlangShaderReflector::AddPushConstant(slang::ProgramLayout& layout,
        slang::VariableLayoutReflection& variable, ShaderReflectionData& data)
    {
        slang::TypeLayoutReflection* typeLayout = variable.getTypeLayout();
        if (!typeLayout) return;

        ShaderReflectionData::PushConstant constant{};
        constant.Name = variable.getName() ? variable.getName() : "";
        constant.Offset = static_cast<uint32_t>(variable.getOffset(slang::ParameterCategory::PushConstantBuffer));
        constant.Size = typeLayout->getElementTypeLayout()
            ? static_cast<uint32_t>(typeLayout->getElementTypeLayout()->getSize())
            : static_cast<uint32_t>(typeLayout->getSize());
        constant.ShaderStage = GetPushConstantStages(layout);
        constant.Binding = variable.getBindingIndex();
        data.PushConstants.push_back(std::move(constant));
    }

    void SlangShaderReflector::AddVertexInput(slang::VariableLayoutReflection& input, ShaderReflectionData& data)
    {
        slang::TypeReflection* type = input.getType();
        slang::TypeLayoutReflection* typeLayout = input.getTypeLayout();
        if (!type || !typeLayout) return;

        // Entry-point inputs are commonly structs. Flatten their fields so each
        // field with a location becomes a separate vertex attribute.
        if (type->getKind() == slang::TypeReflection::Kind::Struct)
        {
            for (unsigned int fieldIndex = 0; fieldIndex < typeLayout->getFieldCount(); ++fieldIndex)
            {
                if (auto* field = typeLayout->getFieldByIndex(fieldIndex))
                    AddVertexInput(*field, data);
            }
            return;
        }

        ShaderReflectionData::VertexAttribute attribute{};
        attribute.Name = input.getName() ? input.getName() : "";
        attribute.Location = input.getBindingIndex();
        attribute.PixelFormat = ToVertexFormat(*type);
        if (attribute.PixelFormat == RHI::Format::Undefined)
        {
            OPAL_WARN("ShaderReflection", "Skipping unsupported vertex input '{}'", attribute.Name);
            return;
        }
        attribute.Size = GetFormatStride(attribute.PixelFormat);
        data.InputAttributes.push_back(std::move(attribute));
    }

    RHI::ShaderStage SlangShaderReflector::ToShaderStage(SlangStage stage)
    {
        switch (stage)
        {
            case SLANG_STAGE_VERTEX: return RHI::ShaderStage::Vertex;
            case SLANG_STAGE_FRAGMENT: return RHI::ShaderStage::Fragment;
            case SLANG_STAGE_COMPUTE: return RHI::ShaderStage::Compute;
            default: return RHI::ShaderStage::Compute;
        }
    }

    RHI::Format SlangShaderReflector::ToVertexFormat(slang::TypeReflection& type)
    {
        const auto element = GetVertexElementType(type);
        if (!element) return RHI::Format::Undefined;

        switch (element->ScalarType->getScalarType())
        {
            case slang::TypeReflection::ScalarType::Float32: return GetFloatFormat(element->ComponentCount, false);
            case slang::TypeReflection::ScalarType::Float16: return GetFloatFormat(element->ComponentCount, true);
            case slang::TypeReflection::ScalarType::Int32:
                return element->ComponentCount == 1 ? RHI::Format::R32_INT
                    : element->ComponentCount == 4 ? RHI::Format::R32G32B32A32_INT : RHI::Format::Undefined;
            case slang::TypeReflection::ScalarType::UInt32:
                return element->ComponentCount == 1 ? RHI::Format::R32_UINT : RHI::Format::Undefined;
            default: return RHI::Format::Undefined;
        }
    }

    uint32_t SlangShaderReflector::GetPushConstantStages(slang::ProgramLayout& layout)
    {
        uint32_t stages = 0;
        for (unsigned int index = 0; index < layout.getEntryPointCount(); ++index)
        {
            switch (layout.getEntryPointByIndex(index)->getStage())
            {
                case SLANG_STAGE_VERTEX: stages |= static_cast<uint32_t>(RHI::ShaderStage::Vertex); break;
                case SLANG_STAGE_FRAGMENT: stages |= static_cast<uint32_t>(RHI::ShaderStage::Fragment); break;
                case SLANG_STAGE_COMPUTE: stages |= static_cast<uint32_t>(RHI::ShaderStage::Compute); break;
                default: break;
            }
        }
        return stages;
    }
}
