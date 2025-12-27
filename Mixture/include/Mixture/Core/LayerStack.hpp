#pragma once

/**
 * @file LayerStack.hpp
 * @brief Manages the stack of layers and overlays.
 */

#include "Mixture/Core/Layer.hpp"

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
        ~LayerStack() = default;

        /**
         * @brief Shuts down the layer stack, detaching all layers.
         */
        void Shutdown() const;

        /**
         * @brief Pushes a layer onto the stack.
         *
         * It is inserted before the overlays.
         *
         * @param layer The layer to push.
         */
        void PushLayer(Layer* layer);

        /**
         * @brief Pushes an overlay onto the stack.
         *
         * It is pushed to the back of the list (rendered last).
         *
         * @param layer The overlay to push.
         */
        void PushOverlay(Layer* layer);

        /**
         * @brief Pops a layer from the stack.
         *
         * @param layer The layer to pop.
         */
        void PopLayer(Layer* layer);

        /**
         * @brief Pops an overlay from the stack.
         *
         * @param layer The overlay to pop.
         */
        void PopOverlay(Layer* layer);

        Vector<Layer*>::iterator begin() { return m_Layers.begin(); }
        Vector<Layer*>::iterator end() { return m_Layers.end(); }
        Vector<Layer*>::reverse_iterator rbegin() { return m_Layers.rbegin(); }
        Vector<Layer*>::reverse_iterator rend() { return m_Layers.rend(); }

        OPAL_NODISCARD Vector<Layer*>::const_iterator begin() const { return m_Layers.begin(); }
        OPAL_NODISCARD Vector<Layer*>::const_iterator end() const { return m_Layers.end(); }
        OPAL_NODISCARD Vector<Layer*>::const_reverse_iterator rbegin() const { return m_Layers.rbegin(); }
        OPAL_NODISCARD Vector<Layer*>::const_reverse_iterator rend() const { return m_Layers.rend(); }

    private:
        Vector<Layer*> m_Layers;
        unsigned int m_LayerInsertIndex = 0;
    };
}
