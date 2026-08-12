#pragma once

#include "Panels/IEditorPanel.hpp"
#include <array>

namespace Mixture
{
    /**
     * @brief Panel responsible for displaying renderer statistics and performance diagnostics.
     */
    class StatsPanel final : public IEditorPanel
    {
    public:
        StatsPanel();
        ~StatsPanel() override = default;

        void OnDrawImGui() override;
        void OnUpdate(float dt) override;

    private:
        std::array<float, 60> m_FrameTimeHistory{};
        size_t m_HistoryIndex = 0;
        float m_TimeAccumulator = 0.0f;
    };
}
