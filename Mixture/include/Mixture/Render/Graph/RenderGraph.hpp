#pragma once

/**
 * @file RenderGraph.hpp
 * @brief Main RenderGraph class for managing passes and resources.
 */

#include "Mixture/Core/Base.hpp"

#include "Mixture/Render/Graph/RenderGraphDefinitions.hpp"
#include "Mixture/Render/Graph/RenderGraphBuilder.hpp"
#include "Mixture/Render/Graph/RenderGraphRegistry.hpp"

namespace Mixture
{

    /**
     * @brief Manages the render graph, including pass creation, compilation, and execution.
     */
    class RenderGraph 
    {
    public:
        RenderGraph() = default;
        
        /**
         * @brief Resets the render graph, clearing all passes and resources.
         * Should be called at the start of each frame.
         */
        void Clear();

        /**
         * @brief Adds a new render pass to the graph.
         * 
         * @tparam DataT A struct defining the pass data (inputs/outputs).
         * @tparam SetupFunc Lambda to declare usage -> (Builder&, DataT&).
         * @tparam ExecFunc Lambda to draw commands -> (Registry&, DataT&, ICommandList*).
         * @param name The name of the pass (for debugging/profiling).
         * @param setup The setup function where resources are declared.
         * @param execute The execution function where commands are recorded.
         * @return DataT& A reference to the pass data structure.
         */
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
            pass.Execute = [=](RenderGraphRegistry& registry, Ref<RHI::ICommandList> cmdList) 
                {
                    execute(registry, *dataPtr, cmdList);
                };

            return *dataPtr;
        }

        /**
         * @brief Compiles the render graph.
         * Calculates resource lifetimes, barriers, and optimizes the graph execution order.
         */
        void Compile();

        /**
         * @brief Executes the compiled render graph.
         * 
         * @param cmdList The command list to record commands into.
         */
        void Execute(Ref<RHI::ICommandList> cmdList);

        /**
         * @brief Imports an external resource (e.g., Swapchain Backbuffer) into the graph.
         * Returns a handle that passes can use to Write() to it.
         * 
         * @param name The name of the resource.
         * @param resource The external texture resource.
         * @return RGResourceHandle A handle to the imported resource.
         */
        RGResourceHandle ImportResource(const std::string& name, Ref<RHI::ITexture> resource);

        /**
         * @brief Creates a new internal resource (transient) for the graph.
         * 
         * @param name The name of the resource.
         * @param desc The description of the texture to create.
         * @return RGResourceHandle A handle to the created resource.
         */
        RGResourceHandle CreateResource(const std::string& name, const RHI::TextureDesc& desc);

        /**
         * @brief Gets the current pass node being processed.
         * Internal getter for the Builder.
         * 
         * @return RGPassNode& Reference to the current pass node.
         */
        RGPassNode& GetCurrentPass() { return m_Passes.back(); }

    private:
        void SortPasses();
        void CalculateLifetimes();
        void CalculateBarriers();

    private:
        Vector<RGPassNode> m_Passes;
        Vector<RGTextureNode> m_Resources;

        RenderGraphRegistry m_Registry;
    };
}
