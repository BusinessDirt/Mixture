#include "mxpch.hpp"
#include "Mixture/Render/Graph/RenderGraphBuilder.hpp"

#include "Mixture/Render/Graph/RenderGraph.hpp"
#include "Mixture/Render/PipelineCache.hpp"
#include "Mixture/Render/ShaderLibrary.hpp"
#include "Mixture/Assets/AssetManager.hpp"

namespace Mixture
{
    RenderGraphBuilder::RenderGraphBuilder(RenderGraph& graph, RGPassNode& passNode)
        : m_Graph(graph), m_PassNode(passNode)
    {}

    RGResourceHandle RenderGraphBuilder::Read(RGResourceHandle handle)
    {
        if (!handle.IsValid())
        {
            OPAL_WARN("Core/RenderGraph", "RenderGraphBuilder::Read - RGResourceHandle is invalid");
            return handle;
        }

        // Record that this pass READS this resource
        m_PassNode.Reads.push_back(handle);
        return handle;
    }

    RGResourceHandle RenderGraphBuilder::Write(RGResourceHandle handle)
    {
        RGAttachmentInfo info;
        info.Handle = handle;
        return Write(info);
    }

    // TODO: Future Proofing: If implementing resource versioning (renaming),
    // this is where a NEW handle ID would be returned.
    // For now, we return the same one.
    RGResourceHandle RenderGraphBuilder::Write(const RGAttachmentInfo& info)
    {
        if (!info.Handle.IsValid())
        {
            OPAL_WARN("Core/RenderGraph", "RenderGraphBuilder::Write - RGResourceHandle is invalid");
            return info.Handle;
        }

        m_PassNode.Writes.push_back(info);
        const RHI::TextureDesc& desc = m_Graph.GetResourceDesc(info.Handle);

        if (RHI::IsDepthFormat(desc.Format))
        {
            m_CurrentDepthFormat = desc.Format;
        }
        else
        {
            m_CurrentColorFormats.push_back(desc.Format);
        }

        return info.Handle;
    }

    RGResourceHandle RenderGraphBuilder::CreateTexture(const std::string& name, const RHI::TextureDesc& desc)
    {
        // Delegate the actual allocation logic to the main graph
        return m_Graph.CreateResource(name, desc);
    }

    RHI::IShader* RenderGraphBuilder::LoadShader(const std::string& path, RHI::ShaderStage stage)
    {
        AssetHandle handle = AssetManager::Get().GetAsset(AssetType::Shader, path);
        RHI::IShader* shader = ShaderLibrary::GetShader(handle, stage);
        return shader;
    }

    RHI::IPipeline* RenderGraphBuilder::CreatePipeline(RHI::PipelineDesc& desc)
    {
        desc.ColorAttachmentFormats = m_CurrentColorFormats;
        desc.DepthAttachmentFormat = m_CurrentDepthFormat;

        if (desc.ColorAttachmentFormats.empty() && desc.DepthAttachmentFormat == RHI::Format::Undefined)
        {
            OPAL_WARN("Core/RenderGraph", "Creating a pipeline with no output attachments defined in this pass!");
        }

        return PipelineCache::GetPipeline(desc);
    }
}
