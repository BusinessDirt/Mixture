#pragma once

#include "Mixture/Core/Base.hpp"

#include <string>
#include <cstdint>

namespace Mixture
{
    /**
     * @brief Data structure holding real-time rendering statistics.
     */
    struct RenderStatsData
    {
        uint32_t DrawCalls = 0;
        uint32_t VertexCount = 0;
        uint32_t TriangleCount = 0;
        uint32_t RenderPassCount = 0;

        float FrameTimeMs = 0.0f;
        float FPS = 0.0f;

        float VRAMUsageMB = 0.0f;
        float SystemRAMUsageMB = 0.0f;

        std::string GraphicsAPI = "Vulkan 1.3";
    };

    /**
     * @brief Singleton collector for engine rendering metrics.
     * 
     * Controlled by OPAL_DIST: in distribution/release builds, all tracking
     * operations compile into no-ops for optimal runtime performance.
     */
    class RenderStats
    {
    public:
        RenderStats() = default;
        ~RenderStats() = default;

        /** Gets the singleton RenderStats instance. */
        static RenderStats& Get();

#if !defined(OPAL_DIST)
        /** Resets per-frame counters at the start of a new frame. */
        void ResetFrameStats();

        /** Records a non-indexed draw call. */
        void RecordDraw(uint32_t vertexCount, uint32_t instanceCount = 1);

        /** Records an indexed draw call. */
        void RecordDrawIndexed(uint32_t indexCount, uint32_t instanceCount = 1);

        /** Records a render pass execution. */
        void RecordRenderPass();

        /** Updates timing and framerate metrics. */
        void UpdateFrameTiming(float frameTimeMs, float fps);

        /** Sets current graphics API name. */
        void SetGraphicsAPI(std::string apiName);

        /** Sets current memory utilization metrics. */
        void SetMemoryUsage(float vramMB, float ramMB);

        /** Gets current frame statistics data. */
        OPAL_NODISCARD const RenderStatsData& GetStats() const { return m_FrameStats; }

#else
        // Compiled out in OPAL_DIST builds for zero performance cost
        inline void ResetFrameStats() {}
        inline void RecordDraw(uint32_t, uint32_t = 1) {}
        inline void RecordDrawIndexed(uint32_t, uint32_t = 1) {}
        inline void RecordRenderPass() {}
        inline void UpdateFrameTiming(float, float) {}
        inline void SetGraphicsAPI(std::string) {}
        inline void SetMemoryUsage(float, float) {}

        OPAL_NODISCARD inline RenderStatsData GetStats() const { return {}; }
#endif

    private:
        RenderStatsData m_FrameStats{};
    };
}
