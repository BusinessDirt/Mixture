#pragma once

/**
 * @file RenderGraph.hpp
 * @brief Main RenderGraph class for managing passes and resources.
 */

#include "Mixture/Core/Base.hpp"
#include "Mixture/Core/LinearAllocator.hpp"

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
        RenderGraph() : m_PassAllocator(64 * 1024) {}

        /**
         * @brief Resets the render graph, clearing all passes and resources.
         * Should be called at the start of each frame.
         */
        void Clear();

        /**
         * @brief Adds a new render pass to the graph.
         *
         * @tparam DataT A struct defining the pass data (inputs/outputs).
         * @param name The name of the pass (for debugging/profiling).
         * @param setup The setup function where resources are declared.
         * @param execute The execution function where commands are recorded.
         */
        template<typename PassData>
        void AddPass(const std::string& name,
             std::function<void(RenderGraphBuilder&, PassData&)> setup,
             std::function<void(const RenderGraphRegistry&, const PassData&, RHI::ICommandList*)> execute)
        {
            static_assert(std::is_trivially_destructible<PassData>::value, "RenderGraph PassData must be trivially destructible (POD). Do not use std::vector or std::string inside PassData!");

            auto& pass = m_Passes.emplace_back();
            pass.Name = name;

            auto data = m_PassAllocator.Alloc<PassData>();

            RenderGraphBuilder builder(*this, pass);
            setup(builder, *data);

            pass.Execute = [=](RenderGraphRegistry& registry, RHI::ICommandList* cmdList)
                {
                    execute(registry, *data, cmdList);
                };
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
         * @param context The graphics context (for resource creation).
         */
        void Execute(RHI::ICommandList* cmdList, RHI::IGraphicsContext* context);

        /**
         * @brief Imports an external resource (e.g., Swapchain Backbuffer) into the graph.
         * Returns a handle that passes can use to Write() to it.
         *
         * @param name The name of the resource.
         * @param resource The external texture resource.
         * @return RGResourceHandle A handle to the imported resource.
         */
        RGResourceHandle ImportResource(const std::string& name, RHI::ITexture* resource);

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

        /**
         * @brief Retrieves an existing resource handle by name.
         *
         * @param name The name of the resource to find.
         * @return RGResourceHandle The handle if found, or an invalid handle.
         */
        RGResourceHandle GetResource(const std::string& name) const;

        /**
         * @brief Retrieves the description of a resource by handle.
         *
         * @param handle The handle of the resource.
         * @return const RHI::TextureDesc& The description of the resource.
         */
        const RHI::TextureDesc& GetResourceDesc(RGResourceHandle handle) const;

    private:
        void SortPasses();
        void CalculateLifetimes();
        void CalculateBarriers();
        void DumpGraphToJSON();

    private:
        LinearAllocator m_PassAllocator;

        Vector<RGPassNode> m_Passes;
        Vector<RGTextureNode> m_Resources;

        RenderGraphRegistry m_Registry;
    };
}
