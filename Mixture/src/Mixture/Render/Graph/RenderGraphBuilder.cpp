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
        if (!handle.IsValid())
        {
            OPAL_WARN("Core/RenderGraph", "RenderGraphBuilder::Write - RGResourceHandle is invalid");
            return handle;
        }

        const auto& node = m_Graph.GetResourceNode(handle);
        if (node.Type == RGResourceType::Buffer || node.Type == RGResourceType::ImportedBuffer)
        {
            m_PassNode.BufferWrites.push_back(handle);
            return handle;
        }
        else
        {
            RGAttachmentInfo info;
            info.Handle = handle;
            return Write(info);
        }
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
        const RHI::TextureDesc& desc = m_Graph.GetTextureDesc(info.Handle);

        if (RHI::IsDepthFormat(desc.PixelFormat))
        {
            m_CurrentDepthFormat = desc.PixelFormat;
        }
        else
        {
            m_CurrentColorFormats.push_back(desc.PixelFormat);
        }

        return info.Handle;
    }

    void RenderGraphBuilder::SetSideEffect()
    {
        m_PassNode.HasSideEffects = true;
    }

    RGResourceHandle RenderGraphBuilder::CreateTexture(const std::string& name, const RHI::TextureDesc& desc)
    {
        // Delegate the actual allocation logic to the main graph
        return m_Graph.CreateResource(name, desc);
    }

    RGResourceHandle RenderGraphBuilder::CreateBuffer(const std::string& name, const RHI::BufferDesc& desc)
    {
        return m_Graph.CreateResource(name, desc);
    }

    RHI::IShader* RenderGraphBuilder::LoadShader(const std::string& path, RHI::ShaderStage stage)
    {
        return LoadShader(ResolveShader(path), stage);
    }

    AssetHandle RenderGraphBuilder::ResolveShader(const std::string& path)
    {
        return AssetManager::Get().GetAsset(AssetType::Shader, path);
    }

    RHI::IShader* RenderGraphBuilder::LoadShader(AssetHandle handle, RHI::ShaderStage stage)
    {
        return ShaderLibrary::GetShader(handle, stage);
    }

    RHI::IPipeline* RenderGraphBuilder::CreatePipeline(RHI::PipelineDesc& desc)
    {
        // If shader assets are not loaded yet, we can't create the pipeline.
        if (desc.VertexShader == nullptr)
        {
            return nullptr;
        }

        desc.ColorAttachmentFormats = m_CurrentColorFormats;
        desc.DepthAttachmentFormat = m_CurrentDepthFormat;

        if (desc.ColorAttachmentFormats.empty() && desc.DepthAttachmentFormat == RHI::Format::Undefined)
        {
            OPAL_WARN("Core/RenderGraph", "Creating a pipeline with no output attachments defined in this pass!");
        }

        return PipelineCache::GetPipeline(desc);
    }
}
