#include "mxpch.hpp"
#include "Mixture/Core/LayerStack.hpp"

#include <algorithm>
#include <stdexcept>

namespace Mixture
{
    LayerStack::~LayerStack()
    {
        Shutdown();
    }

    void LayerStack::Shutdown()
    {
        for (const auto& layer : m_Layers)
            layer->OnDetach();
        m_Layers.clear();
        m_LayerInsertIndex = 0;
    }

    Layer& LayerStack::PushLayer(Scope<Layer> layer)
    {
        if (!layer) throw std::invalid_argument("Cannot push a null layer");
        Layer& result = *layer;
        m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, std::move(layer));
        m_LayerInsertIndex++;
        result.OnAttach();
        return result;
    }

    Layer& LayerStack::PushOverlay(Scope<Layer> layer)
    {
        if (!layer) throw std::invalid_argument("Cannot push a null overlay");
        Layer& result = *layer;
        m_Layers.emplace_back(std::move(layer));
        result.OnAttach();
        return result;
    }

    Scope<Layer> LayerStack::PopLayer(Layer* layer)
    {
        if (const auto it = std::find_if(m_Layers.begin(), m_Layers.end(),
                [layer](const Scope<Layer>& candidate) { return candidate.get() == layer; });
            it != m_Layers.end() && it < m_Layers.begin() + m_LayerInsertIndex)
        {
            layer->OnDetach();
            Scope<Layer> removed = std::move(*it);
            m_Layers.erase(it);
            m_LayerInsertIndex--;
            return removed;
        }
        return nullptr;
    }

    Scope<Layer> LayerStack::PopOverlay(Layer* layer)
    {
        if (const auto it = std::find_if(m_Layers.begin(), m_Layers.end(),
                [layer](const Scope<Layer>& candidate) { return candidate.get() == layer; });
            it != m_Layers.end() && it >= m_Layers.begin() + m_LayerInsertIndex)
        {
            layer->OnDetach();
            Scope<Layer> removed = std::move(*it);
            m_Layers.erase(it);
            return removed;
        }
        return nullptr;
    }
}
