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
            // A. Create the Pass Node
            auto& pass = m_Passes.emplace_back();
            pass.Name = name;

            // B. Allocate the Data Packet for this pass
            // (In a real engine, use a linear allocator here to keep memory tight)
            auto dataPtr = new DataT(); // Simplified for now
            
            // C. Run Setup Phase (User declares Read/Writes)
            RenderGraphBuilder builder(*this, pass);
            setup(builder, *dataPtr);

            // D. Store the Execute Lambda
            // We wrap the user's typed lambda into a generic one
            pass.Execute = [=](RenderGraphRegistry& registry, RHI::ICommandList* cmdList) {
                execute(registry, *dataPtr, cmdList);
            };

            return *dataPtr;
        }

        // Compiler & Executor
        void Compile();
        void Execute(RHI::ICommandList* cmdList);

        // Internal getters for the Builder
        RGResourceHandle CreateResource(const std::string& name, const RHI::TextureDesc& desc);
        RGPassNode& GetCurrentPass() { return m_Passes.back(); }

    private:
        Vector<RGPassNode> m_Passes;
        Vector<RGTextureNode> m_Resources;
        RenderGraphRegistry m_Registry; // Maps Handles -> Real Vulkan Objects
    };
}
