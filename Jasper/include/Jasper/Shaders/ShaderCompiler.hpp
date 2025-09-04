#pragma once

#include "Opal/Base.hpp"

#include "Jasper/Shaders/SPVShader.hpp"
#include "Jasper/Shaders/Types/ShaderStageFlagBits.hpp"

namespace Jasper
{
    class ShaderCompiler
    {
    public:
        enum TargetEnvironment : uint8_t
        {
            None = 0, Vulkan, OpenGL, DirectX
        };
        struct Flags
        {
            bool Debug = true;
            TargetEnvironment Environment = None;
        };
        
    public:
        explicit ShaderCompiler(const Flags& flags = {});
        ~ShaderCompiler() = default;

        OPAL_NODISCARD std::vector<uint32_t> CompileSPV(const std::string& source, const std::filesystem::path& path, ShaderStageFlagBits stage) const;
        void ReflectSPV(SPVShader& shader, ShaderStageFlagBits stage) const;

    private:
        Flags m_Flags = {};
    };
}
