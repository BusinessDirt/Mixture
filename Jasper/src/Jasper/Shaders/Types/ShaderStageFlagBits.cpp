#include "Jasper/Shaders/Types/ShaderStageFlagBits.hpp"

#include <Opal/Base.hpp>

#include <shaderc/shaderc.h>

namespace Jasper::Util
{
    ShaderStageFlagBits FilePathToShaderStage(const std::filesystem::path& filePath)
    {
        auto filename = filePath.filename().string();
        std::ranges::transform(filename, filename.begin(), [](unsigned char c) { return std::tolower(c); });
        
        if (filename.ends_with(".glsl")) filename = filename.substr(0, filename.size() - 5);

        if (filename.ends_with(".vert"))  return Vertex;
        if (filename.ends_with(".tesc"))  return TessellationControl;
        if (filename.ends_with(".tese"))  return TessellationEvaluation;
        if (filename.ends_with(".geom"))  return Geometry;
        if (filename.ends_with(".frag"))  return Fragment;
        if (filename.ends_with(".comp"))  return Compute;
        if (filename.ends_with(".task"))  return Task;
        if (filename.ends_with(".mesh"))  return Mesh;
        if (filename.ends_with(".rgen"))  return RayGen;
        if (filename.ends_with(".rahit")) return AnyHit;
        if (filename.ends_with(".rchit")) return ClosestHit;
        if (filename.ends_with(".rmiss")) return Miss;
        if (filename.ends_with(".rint"))  return Intersection;
        if (filename.ends_with(".rcall")) return Callable;

        OPAL_ERROR("Core", "Invalid shader file extension: {}", filename.c_str());
        return Vertex;
    }

    const char* ShaderStageToString(const ShaderStageFlagBits stage)
    {
        switch (stage)
        {
            case Vertex: return "Vertex";
            case TessellationControl: return "Tessellation Control";
            case TessellationEvaluation: return "Tessellation Evaluation";
            case Geometry: return "Geometry";
            case Fragment: return "Fragment";
            case Compute: return "Compute";
            case AllGraphics: return "All Graphics";
            case All: return "All Shader Stages";
            case RayGen: return "Ray Generation";
            case AnyHit: return "Any Hit";
            case ClosestHit: return "Closest Hit";
            case Miss: return "Miss";
            case Intersection: return "Intersection";
            case Callable: return "Callable";
            case Task: return "Task";
            case Mesh: return "Mesh";
            case SubpassShadingHuawei: return "SubpassShading Huawei";
            case ClusterCullingHuawei: return "ClusterCulling Huawei";
        }
        return "Unknown";
    }

    ShaderCStage ShaderStageToShaderCStage(const ShaderStageFlagBits stage)
    {
        switch (stage)
        {
            case Vertex: return shaderc_vertex_shader;
            case Fragment: return shaderc_fragment_shader;
            case Compute: return shaderc_compute_shader;
            case Geometry: return shaderc_geometry_shader;
            case TessellationControl: return shaderc_tess_control_shader;
            case TessellationEvaluation: return shaderc_tess_evaluation_shader;
            case RayGen: return shaderc_raygen_shader;
            case AnyHit: return shaderc_anyhit_shader;
            case ClosestHit: return shaderc_closesthit_shader;
            case Miss: return shaderc_miss_shader;
            case Intersection: return shaderc_intersection_shader;
            case Callable: return shaderc_callable_shader;
            case Task: return shaderc_task_shader;
            case Mesh: return shaderc_mesh_shader;
            case SubpassShadingHuawei: 
            case ClusterCullingHuawei:
            case AllGraphics:
            case All:
                return shaderc_glsl_infer_from_source;
        }
        return shaderc_glsl_infer_from_source;
    }
}
