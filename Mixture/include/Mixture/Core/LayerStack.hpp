#pragma once

/**
 * @file LayerStack.hpp
 * @brief Manages the stack of layers and overlays.
 */

#include "Mixture/Core/Layer.hpp"

#include <concepts>

namespace Mixture
{
    /**
     * @brief Manages a stack of layers.
     *
     * Layers are stored in a vector, but split into "Layers" and "Overlays".
     * Overlays are always pushed after regular layers.
     */
    class LayerStack
    {
    public:
        OPAL_NON_COPIABLE(LayerStack);

        LayerStack() = default;
        ~LayerStack();

        /**
         * @brief Shuts down the layer stack, detaching all layers.
         */
        void Shutdown();

        /**
         * @brief Pushes a layer onto the stack.
         *
         * It is inserted before the overlays.
         *
         * @param layer The layer to push.
         */
        Layer& PushLayer(Scope<Layer> layer);

        /**
         * @brief Constructs and pushes a layer directly into stack ownership.
         *
         * @tparam LayerT Concrete Layer-derived type to construct.
         * @param args Arguments forwarded to LayerT's constructor.
         * @return LayerT& Reference to the owned layer.
         */
        template<typename LayerT, typename... Args>
            requires std::derived_from<LayerT, Layer>
        LayerT& PushLayer(Args&&... args)
        {
            auto layer = CreateScope<LayerT>(std::forward<Args>(args)...);
            LayerT& result = *layer;
            PushLayer(std::move(layer));
            return result;
        }

        /**
         * @brief Pushes an overlay onto the stack.
         *
         * It is pushed to the back of the list (rendered last).
         *
         * @param layer The overlay to push.
         */
        Layer& PushOverlay(Scope<Layer> layer);

        /**
         * @brief Constructs and pushes an overlay directly into stack ownership.
         *
         * @tparam LayerT Concrete Layer-derived type to construct.
         * @param args Arguments forwarded to LayerT's constructor.
         * @return LayerT& Reference to the owned overlay.
         */
        template<typename LayerT, typename... Args>
            requires std::derived_from<LayerT, Layer>
        LayerT& PushOverlay(Args&&... args)
        {
            auto layer = CreateScope<LayerT>(std::forward<Args>(args)...);
            LayerT& result = *layer;
            PushOverlay(std::move(layer));
            return result;
        }

        /**
         * @brief Pops a layer from the stack.
         *
         * @param layer The layer to pop.
         */
        Scope<Layer> PopLayer(Layer* layer);

        /**
         * @brief Pops an overlay from the stack.
         *
         * @param layer The overlay to pop.
         */
        Scope<Layer> PopOverlay(Layer* layer);

        /**
         * @brief Iterator to the beginning of the layer stack.
         */
        Vector<Scope<Layer>>::iterator begin() { return m_Layers.begin(); }

        /**
         * @brief Iterator to the end of the layer stack.
         */
        Vector<Scope<Layer>>::iterator end() { return m_Layers.end(); }

        /**
         * @brief Reverse iterator to the beginning of the layer stack.
         */
        Vector<Scope<Layer>>::reverse_iterator rbegin() { return m_Layers.rbegin(); }

        /**
         * @brief Reverse iterator to the end of the layer stack.
         */
        Vector<Scope<Layer>>::reverse_iterator rend() { return m_Layers.rend(); }

        /**
         * @brief Const iterator to the beginning of the layer stack.
         */
        OPAL_NODISCARD Vector<Scope<Layer>>::const_iterator begin() const { return m_Layers.begin(); }

        /**
         * @brief Const iterator to the end of the layer stack.
         */
        OPAL_NODISCARD Vector<Scope<Layer>>::const_iterator end() const { return m_Layers.end(); }

        /**
         * @brief Const reverse iterator to the beginning of the layer stack.
         */
        OPAL_NODISCARD Vector<Scope<Layer>>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }

        /**
         * @brief Const reverse iterator to the end of the layer stack.
         */
        OPAL_NODISCARD Vector<Scope<Layer>>::const_reverse_iterator rend() const { return m_Layers.rend(); }

    private:
        Vector<Scope<Layer>> m_Layers;
        unsigned int m_LayerInsertIndex = 0;
    };
}
