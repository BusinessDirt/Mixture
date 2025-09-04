#pragma once

#include <filesystem>

namespace Jasper
{
    
    enum ShaderStageFlagBits
    {
        Vertex = 0x00000001,
        TessellationControl = 0x00000002,
        TessellationEvaluation = 0x00000004,
        Geometry = 0x00000008,
        Fragment = 0x00000010,
        Compute = 0x00000020,
        AllGraphics = 0x0000001F,
        All = 0x7FFFFFFF,
        RayGen = 0x00000100,
        AnyHit = 0x00000200,
        ClosestHit = 0x00000400,
        Miss = 0x00000800,
        Intersection = 0x00001000,
        Callable = 0x00002000,
        Task = 0x00000040,
        Mesh = 0x00000080,
        SubpassShadingHuawei = 0x00004000,
        ClusterCullingHuawei = 0x00080000
    };

    namespace Util
    {
        using ShaderCStage = int;
        ShaderStageFlagBits FilePathToShaderStage(const std::filesystem::path& filePath);
        const char* ShaderStageToString(ShaderStageFlagBits stage);
        ShaderCStage ShaderStageToShaderCStage(ShaderStageFlagBits stage);
    }
}
