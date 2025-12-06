#include "Jasper/Shaders/SPVShader.hpp"

#include "Jasper/Util/Util.hpp"

#define LIST_ITEM_BLANK " [-] "

#define INFO_HORIZONTAL_BAR "=========================="
#define INFO_BEGIN(title, ...) OPAL_INFO("Core", ""); OPAL_INFO("Core", "{}:", title, __VA_ARGS__); OPAL_INFO("Core", INFO_HORIZONTAL_BAR)
#define INFO_LIST(text, tabs, ...) OPAL_INFO("Core", fmt::runtime(std::string(static_cast<size_t>(tabs) * 2, ' ') + std::string(LIST_ITEM_BLANK) + (text)), __VA_ARGS__)
#define INFO_LIST_HEADER(text, tabs) OPAL_INFO("Core", fmt::runtime(std::string(static_cast<size_t>(tabs) * 2, ' ') + std::string(LIST_ITEM_BLANK) + (text)))
#define INFO_END() OPAL_INFO("Core", INFO_HORIZONTAL_BAR)

namespace Jasper::Util
{
    void PrintSPVShaderDebugInformation(const SPVShader& shader)
    {
        INFO_BEGIN("SPVShader '{}'", shader.Name);
        //INFO_LIST("Pipeline Type: {}", 0, shader..PipelineType == Graphics ? "Graphics" : "Unknown");

        // Push Constant
        if (shader.PushConstant.Size > 0)
        {
            INFO_LIST_HEADER("Push Constant", 0);
            INFO_LIST("Size: {}", 1, shader.PushConstant.Size);
            INFO_LIST("Offset: {}", 1, shader.PushConstant.Offset);
        }
        
        // Descriptor Set Layout Bindings
        if (!shader.DescriptorSetLayoutBindings.empty())
        {
            INFO_LIST_HEADER("Descriptor Set Layout Bindings", 0);
            
            for (const auto& [setIndex, bindings] : shader.DescriptorSetLayoutBindings)
                {
                    for (const auto& binding : bindings)
                    {
                        INFO_LIST("set = {}, binding = {}, type = {}, count = {}", 1, setIndex, binding.Binding,
                            Util::DescriptorTypeToString(binding.Type), binding.Count);
                    }
                }
        }

        // Vertex Input Bindings
        if (!shader.VertexInputBindings.empty())
        {
            INFO_LIST_HEADER("Vertex Input Bindings", 0);
            
            for (const auto& binding : shader.VertexInputBindings)
            {
                INFO_LIST("Binding {}:", 1, binding.Binding);
                INFO_LIST("Stride: {}", 2, binding.Stride);
            }
        }

        // Vertex Input Attributes
        if (!shader.VertexInputAttributes.empty())
        {
            INFO_LIST_HEADER("Vertex Attributes", 0);

            int i = 0;
            for (const auto& [location, binding, format, offset] : shader.VertexInputAttributes)
            {
                INFO_LIST("layout(location = {}, binding = {}, offset = {}) in {} {};", 1, location, binding, offset,
                    Util::FormatToString(format), shader.VertexAttributeNames[i]);
                i++;
            }
        }

        INFO_END();
    }

    size_t HashSPVShader(const SPVShader& shader)
    {
        size_t hash = 0;
        for (const auto& [stage, spirv] : shader.Data)
        {
            HashCombine(hash, std::hash<int>{}(static_cast<int>(stage)));
            for (uint32_t word : spirv) HashCombine(hash, std::hash<uint32_t>{}(word));
        }
        
        return hash;
    }
}

