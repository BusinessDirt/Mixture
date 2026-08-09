#include "mxpch.hpp"
#include "Mixture/Render/Graph/RenderGraph.hpp"

#include "Mixture/Core/Application.hpp"
#include "Mixture/Render/RHI/IGraphicsContext.hpp"
#include "Mixture/Render/RHI/IGraphicsDevice.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace Mixture
{
    namespace
    {
        class JSONWriter
        {
        public:
            explicit JSONWriter(std::ostream& output) : m_Output(output) {}

            void BeginObject() { BeforeValue(); m_Output << '{'; m_Contexts.push_back({ Type::Object }); }
            void EndObject() { EndContainer('}'); }
            void BeginArray() { BeforeValue(); m_Output << '['; m_Contexts.push_back({ Type::Array }); }
            void EndArray() { EndContainer(']'); }

            void Key(std::string_view key)
            {
                auto& context = m_Contexts.back();
                WriteSeparator(context);
                WriteString(key);
                m_Output << ": ";
                context.ExpectingValue = true;
            }

            void String(std::string_view value) { BeforeValue(); WriteString(value); }
            void Number(uint64_t value) { BeforeValue(); m_Output << value; }

        private:
            enum class Type { Object, Array };
            struct Context
            {
                Type ContainerType;
                bool First = true;
                bool ExpectingValue = false;
            };

            void BeforeValue()
            {
                if (m_Contexts.empty()) return;
                auto& context = m_Contexts.back();
                if (context.ContainerType == Type::Object && context.ExpectingValue)
                {
                    context.ExpectingValue = false;
                    return;
                }
                WriteSeparator(context);
            }

            void WriteSeparator(Context& context)
            {
                if (!context.First) m_Output << ',';
                m_Output << '\n' << std::string(m_Contexts.size() * 2, ' ');
                context.First = false;
            }

            void EndContainer(char delimiter)
            {
                const bool hadValues = !m_Contexts.back().First;
                m_Contexts.pop_back();
                if (hadValues) m_Output << '\n' << std::string(m_Contexts.size() * 2, ' ');
                m_Output << delimiter;
            }

            void WriteString(std::string_view value)
            {
                static constexpr char HexDigits[] = "0123456789abcdef";
                m_Output << '"';
                for (const unsigned char character : value)
                {
                    switch (character)
                    {
                        case '"': m_Output << "\\\""; break;
                        case '\\': m_Output << "\\\\"; break;
                        case '\b': m_Output << "\\b"; break;
                        case '\f': m_Output << "\\f"; break;
                        case '\n': m_Output << "\\n"; break;
                        case '\r': m_Output << "\\r"; break;
                        case '\t': m_Output << "\\t"; break;
                        default:
                            if (character < 0x20)
                            {
                                m_Output << "\\u00"
                                         << HexDigits[(character >> 4) & 0x0f]
                                         << HexDigits[character & 0x0f];
                            }
                            else m_Output << static_cast<char>(character);
                    }
                }
                m_Output << '"';
            }

            std::ostream& m_Output;
            Vector<Context> m_Contexts;
        };
    }

    void RenderGraphAlgorithms::CullPasses(Vector<RGPassNode>& passes, const Vector<RGResourceNode>& resources)
    {
        std::unordered_set<RGResourceHandle::IDType> requiredResources;
        Vector<bool> livePasses(passes.size(), false);

        auto isImported = [&](RGResourceHandle handle)
        {
            if (!handle.IsValid() || handle.ID >= resources.size()) return false;
            const RGResourceType type = resources[handle.ID].Type;
            return type == RGResourceType::ImportedTexture || type == RGResourceType::ImportedBuffer;
        };

        auto isRequired = [&](RGResourceHandle handle)
        {
            return handle.IsValid() && requiredResources.contains(handle.ID);
        };

        for (size_t passIndex = passes.size(); passIndex-- > 0;)
        {
            const auto& pass = passes[passIndex];
            const bool hasWrites = !pass.Writes.empty() || !pass.BufferWrites.empty();

            bool writesImportedResource = false;
            bool producesRequiredResource = false;

            for (const auto& write : pass.Writes)
            {
                writesImportedResource |= isImported(write.Handle);
                producesRequiredResource |= isRequired(write.Handle);
            }
            for (const auto handle : pass.BufferWrites)
            {
                writesImportedResource |= isImported(handle);
                producesRequiredResource |= isRequired(handle);
            }

            const bool isLive = pass.HasSideEffects || !hasWrites ||
                                writesImportedResource || producesRequiredResource;
            if (!isLive) continue;

            livePasses[passIndex] = true;

            // The latest live writer satisfies the downstream requirement.
            for (const auto& write : pass.Writes)
            {
                requiredResources.erase(write.Handle.ID);
                if (write.LoadOp == RHI::LoadOp::Load)
                {
                    requiredResources.insert(write.Handle.ID);
                }
            }
            for (const auto handle : pass.BufferWrites)
            {
                requiredResources.erase(handle.ID);
            }

            for (const auto handle : pass.Reads)
            {
                if (handle.IsValid()) requiredResources.insert(handle.ID);
            }
        }

        Vector<RGPassNode> culledPasses;
        culledPasses.reserve(passes.size());
        for (size_t passIndex = 0; passIndex < passes.size(); ++passIndex)
        {
            if (livePasses[passIndex])
            {
                culledPasses.push_back(std::move(passes[passIndex]));
            }
        }
        passes = std::move(culledPasses);
    }

    bool RenderGraphAlgorithms::SortPasses(Vector<RGPassNode>& passes)
    {
        const size_t passCount = passes.size();
        if (passCount < 2) return true;

        Vector<Vector<size_t>> adjacencyList(passCount);
        Vector<std::unordered_set<size_t>> adjacencySets(passCount);
        Vector<size_t> inDegree(passCount, 0);

        std::unordered_map<RGResourceHandle::IDType, size_t> lastWriters;
        std::unordered_map<RGResourceHandle::IDType, Vector<size_t>> readersSinceLastWrite;

        auto addDependency = [&](size_t before, size_t after)
        {
            if (before == after) return;

            if (adjacencySets[before].insert(after).second)
            {
                adjacencyList[before].push_back(after);
                ++inDegree[after];
            }
        };

        auto registerRead = [&](RGResourceHandle handle, size_t passIndex)
        {
            const auto writer = lastWriters.find(handle.ID);
            if (writer != lastWriters.end())
            {
                // Read-after-write (RAW).
                addDependency(writer->second, passIndex);
            }

            auto& readers = readersSinceLastWrite[handle.ID];
            if (std::find(readers.begin(), readers.end(), passIndex) == readers.end())
            {
                readers.push_back(passIndex);
            }
        };

        auto registerWrite = [&](RGResourceHandle handle, size_t passIndex)
        {
            const auto writer = lastWriters.find(handle.ID);
            if (writer != lastWriters.end())
            {
                // Write-after-write (WAW).
                addDependency(writer->second, passIndex);
            }

            auto& readers = readersSinceLastWrite[handle.ID];
            for (const size_t reader : readers)
            {
                // Write-after-read (WAR).
                addDependency(reader, passIndex);
            }
            readers.clear();

            lastWriters[handle.ID] = passIndex;
        };

        for (size_t passIndex = 0; passIndex < passCount; ++passIndex)
        {
            const auto& pass = passes[passIndex];

            for (const auto handle : pass.Reads)
            {
                registerRead(handle, passIndex);
            }
            for (const auto& write : pass.Writes)
            {
                registerWrite(write.Handle, passIndex);
            }
            for (const auto handle : pass.BufferWrites)
            {
                registerWrite(handle, passIndex);
            }
        }

        // Always choose the earliest declared ready pass. This keeps independent
        // passes stable while still honoring every resource dependency.
        std::priority_queue<size_t, Vector<size_t>, std::greater<size_t>> readyPasses;
        for (size_t passIndex = 0; passIndex < passCount; ++passIndex)
        {
            if (inDegree[passIndex] == 0) readyPasses.push(passIndex);
        }

        Vector<size_t> sortedIndices;
        sortedIndices.reserve(passCount);

        while (!readyPasses.empty())
        {
            const size_t passIndex = readyPasses.top();
            readyPasses.pop();
            sortedIndices.push_back(passIndex);

            for (const size_t dependentPass : adjacencyList[passIndex])
            {
                if (--inDegree[dependentPass] == 0)
                {
                    readyPasses.push(dependentPass);
                }
            }
        }

        if (sortedIndices.size() != passCount) return false;

        Vector<RGPassNode> sortedPasses;
        sortedPasses.reserve(passCount);
        for (const size_t passIndex : sortedIndices)
        {
            sortedPasses.push_back(std::move(passes[passIndex]));
        }
        passes = std::move(sortedPasses);
        return true;
    }

    void RenderGraphAlgorithms::CalculateResourceLifetimes(
        const Vector<RGPassNode>& passes,
        Vector<RGResourceNode>& resources)
    {
        for (auto& resource : resources)
        {
            resource.FirstPassIndex = -1;
            resource.LastPassIndex = -1;
        }

        auto updateResource = [&](RGResourceHandle handle, int32_t passIndex)
        {
            if (!handle.IsValid() || handle.ID >= resources.size()) return;

            auto& resource = resources[handle.ID];
            if (resource.FirstPassIndex == -1) resource.FirstPassIndex = passIndex;
            resource.LastPassIndex = passIndex;
        };

        for (int32_t passIndex = 0; passIndex < static_cast<int32_t>(passes.size()); ++passIndex)
        {
            const auto& pass = passes[passIndex];
            for (const auto handle : pass.Reads) updateResource(handle, passIndex);
            for (const auto& write : pass.Writes) updateResource(write.Handle, passIndex);
            for (const auto handle : pass.BufferWrites) updateResource(handle, passIndex);
        }
    }

    void RenderGraph::Clear()
    {
        m_PassAllocator.Reset();
        m_Passes.clear();
        m_Resources.clear();
        m_ResourceLookup.clear();
        m_ResourcesEndingAtPass.clear();
        m_Registry.Clear();
        // Note: m_Cache is NOT cleared here, to support persistence across frames.
    }

    void RenderGraph::Compile()
    {
        CullPasses();
        SortPasses();
        CalculateLifetimes();
        CalculateBarriers();

        m_ResourcesEndingAtPass.clear();
        m_ResourcesEndingAtPass.resize(m_Passes.size());
        for (const auto& node : m_Resources)
        {
            const bool isTransient = node.Type == RGResourceType::Texture || node.Type == RGResourceType::Buffer;
            if (isTransient && node.LastPassIndex >= 0)
            {
                m_ResourcesEndingAtPass[static_cast<size_t>(node.LastPassIndex)].push_back(node.Handle);
            }
        }
    }

    void RenderGraph::Execute(RHI::ICommandList* cmdList, RHI::IGraphicsContext* context)
    {
        m_Cache.BeginFrame(context->GetCurrentFrameIndex());

        // Realize Resources (Allocation Phase)
        for (const auto& node : m_Resources)
        {
            if (node.FirstPassIndex < 0) continue;

            if (node.Type == RGResourceType::ImportedTexture)
            {
                m_Registry.ImportTexture(node.Handle, node.ExternalTexture);
            }
            else if (node.Type == RGResourceType::ImportedBuffer)
            {
                m_Registry.ImportBuffer(node.Handle, node.ExternalBuffer);
            }
            else if (node.Type == RGResourceType::Texture)
            {
                // Get from Cache
                auto texture = m_Cache.GetOrCreateTexture(node.TextureDesc);
                m_Registry.RegisterTexture(node.Handle, texture.get());
            }
            else if (node.Type == RGResourceType::Buffer)
            {
                // Get from Cache
                auto buffer = m_Cache.GetOrCreateBuffer(node.BufferDesc);
                m_Registry.RegisterBuffer(node.Handle, buffer.get());
            }
        }

        // Execute Passes
        for (size_t passIndex = 0; passIndex < m_Passes.size(); ++passIndex)
        {
            const auto& pass = m_Passes[passIndex];

            // Execute Barriers
            for (const auto& barrier : pass.Barriers)
            {
                auto& resNode = m_Resources[barrier.Resource.ID];
                if (resNode.Type == RGResourceType::Texture || resNode.Type == RGResourceType::ImportedTexture)
                {
                    cmdList->PipelineBarrier(
                        m_Registry.GetTexture(barrier.Resource),
                        barrier.Before,
                        barrier.After
                    );
                }
                else
                {
                    // Buffer Barriers
                    // TODO: cmdList->BufferBarrier(...)
                }
            }

            // Run Logic
            if (pass.Execute)
            {
                RHI::RenderingInfo renderingInfo;

                // Setup Render Area from Attachment Writes
                if (!pass.Writes.empty())
                {
                    RHI::ITexture* firstTex = m_Registry.GetTexture(pass.Writes[0].Handle);
                    if (firstTex)
                    {
                        renderingInfo.RenderAreaWidth = firstTex->GetWidth();
                        renderingInfo.RenderAreaHeight = firstTex->GetHeight();
                    }
                }
                else
                {
                    // Fallback
                    renderingInfo.RenderAreaWidth = context->GetSwapchainWidth();
                    renderingInfo.RenderAreaHeight = context->GetSwapchainHeight();
                }

                // Populate Attachments
                RHI::RenderingAttachment depthAttachmentTemp;
                renderingInfo.DepthAttachment = nullptr;

                for (const auto& write : pass.Writes)
                {
                    // Look up the actual RHI Texture from the handle
                    auto texture = m_Registry.GetTexture(write.Handle);

                    // Create the attachment struct
                    RHI::RenderingAttachment attachment;
                    attachment.Image = texture;
                    attachment.LoadOp = write.LoadOp;
                    attachment.StoreOp = write.StoreOp;

                    // Copy Clear Color
                    memcpy(attachment.ClearColor, write.ClearColor, sizeof(float) * 4);
                    attachment.DepthClearValue = write.DepthClearValue;

                    // Sort into Color vs Depth
                    if (IsDepthFormat(texture->GetFormat()))
                    {
                        depthAttachmentTemp = attachment;
                        renderingInfo.DepthAttachment = &depthAttachmentTemp;
                    }
                    else
                    {
                        renderingInfo.ColorAttachments.push_back(attachment);
                    }
                }

                // Only call BeginRendering if we have attachments
                if (!renderingInfo.ColorAttachments.empty() || renderingInfo.DepthAttachment)
                {
                    uint32_t width = renderingInfo.RenderAreaWidth;
                    uint32_t height = renderingInfo.RenderAreaHeight;

                    cmdList->BeginRendering(renderingInfo);
                    cmdList->SetViewport(0, 0, (float)width, (float)height);
                    cmdList->SetScissor(0, 0, width, height);

                    pass.Execute(m_Registry, cmdList); // Draw commands happen here

                    cmdList->EndRendering();
                }
                else
                {
                    // Just execute without dynamic rendering scope (Compute, Copy, etc.)
                    pass.Execute(m_Registry, cmdList);
                }
            }

            // Virtual handles are valid only through their calculated lifetime.
            // The frame-slot cache retains physical ownership until GPU-safe reuse.
            for (const RGResourceHandle handle : m_ResourcesEndingAtPass[passIndex])
            {
                const auto& resource = m_Resources[handle.ID];
                if (resource.Type == RGResourceType::Texture)
                    m_Registry.UnregisterTexture(handle);
                else
                    m_Registry.UnregisterBuffer(handle);
            }
        }
    }

    RGResourceHandle RenderGraph::ImportResource(const std::string& name, RHI::ITexture* resource)
    {
        RGResourceHandle::IDType id = static_cast<RGResourceHandle::IDType>(m_Resources.size());
        RGResourceHandle handle = { id };

        RGResourceNode node;
        node.Handle = handle;
        node.Name = name;
        node.Type = RGResourceType::ImportedTexture;

        node.TextureDesc.Width = resource->GetWidth();
        node.TextureDesc.Height = resource->GetHeight();
        node.TextureDesc.PixelFormat = resource->GetFormat();
        node.TextureDesc.InitialState = RHI::ResourceState::Undefined; // We don't track external state yet

        node.ExternalTexture = resource;

        m_Resources.push_back(node);
        m_ResourceLookup.try_emplace(name, handle);
        m_Registry.ImportTexture(handle, resource);

        return handle;
    }

    RGResourceHandle RenderGraph::ImportResource(const std::string& name, RHI::IBuffer* resource)
    {
        RGResourceHandle::IDType id = static_cast<RGResourceHandle::IDType>(m_Resources.size());
        RGResourceHandle handle = { id };

        RGResourceNode node;
        node.Handle = handle;
        node.Name = name;
        node.Type = RGResourceType::ImportedBuffer;

        node.BufferDesc.Size = resource->GetSize();
        node.BufferDesc.Usage = resource->GetUsage();

        node.ExternalBuffer = resource;

        m_Resources.push_back(node);
        m_ResourceLookup.try_emplace(name, handle);
        m_Registry.ImportBuffer(handle, resource);

        return handle;
    }

    RGResourceHandle RenderGraph::CreateResource(const std::string& name, const RHI::TextureDesc& desc)
    {
        RGResourceHandle::IDType id = static_cast<RGResourceHandle::IDType>(m_Resources.size());
        RGResourceHandle handle = { id };

        RGResourceNode node;
        node.Handle = handle;
        node.Name = name;
        node.Type = RGResourceType::Texture;
        node.TextureDesc = desc;

        m_Resources.push_back(node);
        m_ResourceLookup.try_emplace(name, handle);
        return handle;
    }

    RGResourceHandle RenderGraph::CreateResource(const std::string& name, const RHI::BufferDesc& desc)
    {
        RGResourceHandle::IDType id = static_cast<RGResourceHandle::IDType>(m_Resources.size());
        RGResourceHandle handle = { id };

        RGResourceNode node;
        node.Handle = handle;
        node.Name = name;
        node.Type = RGResourceType::Buffer;
        node.BufferDesc = desc;

        m_Resources.push_back(node);
        m_ResourceLookup.try_emplace(name, handle);
        return handle;
    }

    RGResourceHandle RenderGraph::GetResource(const std::string& name) const
    {
        const auto resource = m_ResourceLookup.find(name);
        if (resource != m_ResourceLookup.end()) return resource->second;

        OPAL_ERROR("Core/RenderGraph", "Resource not found: %s", name.c_str());
        return RGResourceHandle();
    }

    const RHI::TextureDesc& RenderGraph::GetTextureDesc(RGResourceHandle handle) const
    {
        OPAL_ASSERT("Core/RenderGraph", handle.IsValid() && handle.ID < m_Resources.size(), "Invalid Handle!");
        // TODO: Assert type
        return m_Resources[handle.ID].TextureDesc;
    }

    const RHI::BufferDesc& RenderGraph::GetBufferDesc(RGResourceHandle handle) const
    {
        OPAL_ASSERT("Core/RenderGraph", handle.IsValid() && handle.ID < m_Resources.size(), "Invalid Handle!");
        // TODO: Assert type
        return m_Resources[handle.ID].BufferDesc;
    }

    const RGResourceNode& RenderGraph::GetResourceNode(RGResourceHandle handle) const
    {
        OPAL_ASSERT("Core/RenderGraph", handle.IsValid() && handle.ID < m_Resources.size(), "Invalid Handle!");
        return m_Resources[handle.ID];
    }

    void RenderGraph::SortPasses()
    {
        if (!RenderGraphAlgorithms::SortPasses(m_Passes))
        {
            OPAL_ERROR("Core/RenderGraph", "Failed to produce a valid pass ordering.");
        }
    }

    void RenderGraph::CullPasses()
    {
        RenderGraphAlgorithms::CullPasses(m_Passes, m_Resources);
    }

    void RenderGraph::CalculateLifetimes()
    {
        RenderGraphAlgorithms::CalculateResourceLifetimes(m_Passes, m_Resources);
    }

    void RenderGraph::CalculateBarriers()
    {
        Vector<RHI::ResourceState> currentStates(m_Resources.size());
        Vector<bool> wasLastWrite(m_Resources.size(), false);

        for (size_t i = 0; i < m_Resources.size(); ++i)
        {
            if (m_Resources[i].Type == RGResourceType::Texture || m_Resources[i].Type == RGResourceType::ImportedTexture)
                currentStates[i] = m_Resources[i].TextureDesc.InitialState;
            else
                currentStates[i] = RHI::ResourceState::Undefined;
        }

        for (auto& pass : m_Passes)
        {
            pass.Barriers.clear();

            auto TransitionResource = [&](RGResourceHandle handle, RHI::ResourceState targetState, bool isWrite)
            {
                uint32_t id = handle.ID;
                RHI::ResourceState current = currentStates[id];
                bool previousWasWrite = wasLastWrite[id];

                bool layoutChanged = (current != targetState);
                bool hazardExists = previousWasWrite || (isWrite && current != RHI::ResourceState::Undefined);

                if (!isWrite && !previousWasWrite && !layoutChanged) return;

                if (layoutChanged || hazardExists)
                {
                    RGBarrier barrier;
                    barrier.Resource = handle;
                    barrier.Before = current;
                    barrier.After = targetState;
                    pass.Barriers.push_back(barrier);
                    currentStates[id] = targetState;
                }
                wasLastWrite[id] = isWrite;
            };

            for (auto& handle : pass.Reads)
            {
                auto& node = m_Resources[handle.ID];
                if (node.Type == RGResourceType::Texture || node.Type == RGResourceType::ImportedTexture)
                    TransitionResource(handle, RHI::ResourceState::ShaderResource, false);
                else
                    TransitionResource(handle, RHI::ResourceState::ShaderResource, false); // Or Vertex/Constant based on usage?
            }

            for (auto& info : pass.Writes)
            {
                RGResourceHandle handle = info.Handle;
                auto& node = m_Resources[handle.ID];

                if (node.Type == RGResourceType::Texture || node.Type == RGResourceType::ImportedTexture)
                {
                    bool isDepth = IsDepthFormat(node.TextureDesc.PixelFormat);
                    RHI::ResourceState target = isDepth ? RHI::ResourceState::DepthStencilWrite : RHI::ResourceState::RenderTarget;
                    TransitionResource(handle, target, true);
                }
            }

            for (auto& handle : pass.BufferWrites)
            {
                 TransitionResource(handle, RHI::ResourceState::UnorderedAccess, true);
            }
        }
    }

    bool RenderGraph::DumpDiagnostics(const std::filesystem::path& outputPath) const
    {
        std::error_code error;
        const auto parentPath = outputPath.parent_path();
        if (!parentPath.empty())
        {
            std::filesystem::create_directories(parentPath, error);
            if (error) return false;
        }

        std::ofstream out(outputPath, std::ios::trunc);
        if (!out) return false;

        JSONWriter json(out);
        json.BeginObject();
        json.Key("resources");
        json.BeginArray();
        for (size_t i = 0; i < m_Resources.size(); ++i)
        {
            const auto& resource = m_Resources[i];
            const std::string name = resource.Name.empty() ? "Res_" + std::to_string(i) : resource.Name;
            const bool isTexture = resource.Type == RGResourceType::Texture ||
                                   resource.Type == RGResourceType::ImportedTexture;
            json.BeginObject();
            json.Key("id"); json.Number(i);
            json.Key("name"); json.String(name);
            json.Key("type"); json.String(isTexture ? "Texture" : "Buffer");
            json.EndObject();
        }
        json.EndArray();

        json.Key("passes");
        json.BeginArray();
        for (size_t i = 0; i < m_Passes.size(); ++i)
        {
            const auto& pass = m_Passes[i];
            json.BeginObject();
            json.Key("id"); json.Number(i);
            json.Key("name"); json.String(pass.Name);
            json.Key("barriers");
            json.BeginArray();
            for (const auto& barrier : pass.Barriers)
            {
                json.BeginObject();
                json.Key("res"); json.Number(barrier.Resource.ID);
                json.Key("from"); json.String(RHI::ToString(barrier.Before));
                json.Key("to"); json.String(RHI::ToString(barrier.After));
                json.EndObject();
            }
            json.EndArray();

            json.Key("writes");
            json.BeginArray();
            for (const auto& write : pass.Writes) json.Number(write.Handle.ID);
            json.EndArray();

            json.Key("reads");
            json.BeginArray();
            for (const auto read : pass.Reads) json.Number(read.ID);
            json.EndArray();
            json.EndObject();
        }
        json.EndArray();
        json.EndObject();
        out << '\n';

        if (!out) return false;
        OPAL_LOG_DEBUG("Core/RenderGraph", "Dumped graph diagnostics to '{}'", outputPath.string());
        return true;
    }
}
