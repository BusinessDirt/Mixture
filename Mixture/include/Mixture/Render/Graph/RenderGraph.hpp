#pragma once

#include "Mixture/Core/Base.hpp"

#include "Mixture/Render/Graph/RenderGraphDefinitions.hpp"
#include "Mixture/Render/Graph/RenderGraphBuilder.hpp"
#include "Mixture/Render/Graph/RenderGraphRegistry.hpp"

namespace Mixture
{

    class RenderGraph 
    {
    public:
        RenderGraph() = default;
        
        // Reset everything (call at start of frame)
        void Clear();

        // The Generic "Add Pass" Function
        // DataT: A struct defining the pass data (inputs/outputs)
        // SetupFunc: Lambda to declare usage -> (Builder&, DataT&)
        // ExecFunc: Lambda to draw commands -> (Registry&, DataT&, ICommandList*)
        template<typename DataT, typename SetupFunc, typename ExecFunc>
        DataT& AddPass(const std::string& name, SetupFunc&& setup, ExecFunc&& execute)
        {
            // Create the Pass Node
            auto& pass = m_Passes.emplace_back();
            pass.Name = name;

            // Allocate the Data Packet for this pass
            // TODO: use a linear allocator here to keep memory tight
            auto dataPtr = new DataT(); // Simplified for now
            
            // Run Setup Phase (User declares Read/Writes)
            RenderGraphBuilder builder(*this, pass);
            setup(builder, *dataPtr);

            // Store the Execute Lambda
            // We wrap the user's typed lambda into a generic one
            pass.Execute = [=](RenderGraphRegistry& registry, RHI::ICommandList* cmdList) 
                {
                    execute(registry, *dataPtr, cmdList);
                };

            return *dataPtr;
        }

        // Compiler & Executor
        void Compile();
        void Execute(RHI::ICommandList* cmdList);

        // Import an external resource (like the Swapchain Backbuffer)
        // Returns a handle that passes can use to Write() to it.
        RGResourceHandle ImportResource(const std::string& name, Ref<RHI::ITexture> resource);
        RGResourceHandle CreateResource(const std::string& name, const RHI::TextureDesc& desc);

        // Internal getters for the Builder
        RGPassNode& GetCurrentPass() { return m_Passes.back(); }

    private:
        void SortPasses();
        void CalculateBarriers();

    private:
        Vector<RGPassNode> m_Passes;
        Vector<RGTextureNode> m_Resources;

        RenderGraphRegistry m_Registry;
    };
}
