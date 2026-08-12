#include "mxpch.hpp"
#include "Mixture/Render/RenderStats.hpp"

namespace Mixture
{
    RenderStats& RenderStats::Get()
    {
        static RenderStats instance;
        return instance;
    }

#if !defined(OPAL_DIST)
    void RenderStats::ResetFrameStats()
    {
        m_FrameStats.DrawCalls = 0;
        m_FrameStats.VertexCount = 0;
        m_FrameStats.TriangleCount = 0;
        m_FrameStats.RenderPassCount = 0;
    }

    void RenderStats::RecordDraw(uint32_t vertexCount, uint32_t instanceCount)
    {
        m_FrameStats.DrawCalls++;
        m_FrameStats.VertexCount += vertexCount * instanceCount;
        m_FrameStats.TriangleCount += (vertexCount / 3) * instanceCount;
    }

    void RenderStats::RecordDrawIndexed(uint32_t indexCount, uint32_t instanceCount)
    {
        m_FrameStats.DrawCalls++;
        m_FrameStats.VertexCount += indexCount * instanceCount;
        m_FrameStats.TriangleCount += (indexCount / 3) * instanceCount;
    }

    void RenderStats::RecordRenderPass()
    {
        m_FrameStats.RenderPassCount++;
    }

    void RenderStats::UpdateFrameTiming(float frameTimeMs, float fps)
    {
        m_FrameStats.FrameTimeMs = frameTimeMs;
        m_FrameStats.FPS = fps;
    }

    void RenderStats::SetGraphicsAPI(std::string apiName)
    {
        m_FrameStats.GraphicsAPI = std::move(apiName);
    }

    void RenderStats::SetMemoryUsage(float vramMB, float ramMB)
    {
        m_FrameStats.VRAMUsageMB = vramMB;
        m_FrameStats.SystemRAMUsageMB = ramMB;
    }
#endif
}
