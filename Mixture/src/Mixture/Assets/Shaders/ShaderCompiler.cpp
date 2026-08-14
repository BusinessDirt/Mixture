#include "mxpch.hpp"
#include "Mixture/Assets/Shaders/ShaderCompiler.hpp"
#include "Mixture/Assets/Shaders/SlangShaderReflector.hpp"

#include "Mixture/Assets/AssetManager.hpp"

#include <slang/slang.h>
#include <slang/slang-com-ptr.h>

namespace Mixture
{
    namespace
    {
        struct CompilerState
        {
            Slang::ComPtr<slang::IGlobalSession> GlobalSession;
            bool Available = false;

            CompilerState()
            {
                // Create the Slang global session once during engine initialization
                const SlangResult res = slang::createGlobalSession(GlobalSession.writeRef());
                Available = SLANG_SUCCEEDED(res) && GlobalSession != nullptr;
            }
        };

        CompilerState& GetCompilerState()
        {
            static CompilerState state;
            return state;
        }

        SlangShaderReflector* GetReflector()
        {
            static Scope<SlangShaderReflector> reflector = CreateScope<SlangShaderReflector>();
            return reflector.get();
        }
    }

    bool ShaderCompiler::IsAvailable()
    {
        return GetCompilerState().Available;
    }

    Vector<uint8_t> ShaderCompiler::Compile(const std::string& source)
    {
        return CompileDetailed(source).Bytecode;
    }

    Vector<uint8_t> ShaderCompiler::Compile(const std::string& source, RHI::GraphicsAPI graphicsAPI)
    {
        return CompileDetailed(source, graphicsAPI).Bytecode;
    }

    ShaderCompileResult ShaderCompiler::CompileDetailed(const std::string& source)
    {
        return CompileDetailed(source, AssetManager::Get().GetGraphicsAPI());
    }

    ShaderCompileResult ShaderCompiler::CompileDetailed(const std::string& source, RHI::GraphicsAPI graphicsAPI)
    {
        ShaderCompileResult result;
        if (source.size() > std::numeric_limits<uint32_t>::max())
        {
            result.Diagnostics = "Shader source exceeds size limit";
            return result;
        }

        // Initialize Global Session
        Slang::ComPtr<slang::IGlobalSession> globalSession;
        if (SLANG_FAILED(slang::createGlobalSession(globalSession.writeRef())))
        {
            result.Diagnostics = "Failed to create Slang Global Session";
            return result;
        }

        // Configure the Target
        slang::TargetDesc targetDesc = {};
        targetDesc.profile = globalSession->findProfile("sm_6_5"); // Shader Model 6.5

        switch (graphicsAPI)
        {
            case RHI::GraphicsAPI::Vulkan: targetDesc.format = SLANG_SPIRV; break;
            case RHI::GraphicsAPI::Metal:  targetDesc.format = SLANG_METAL; break;
            case RHI::GraphicsAPI::D3D12:  targetDesc.format = SLANG_DXIL;  break;

            case RHI::GraphicsAPI::None:
            default:
                result.Diagnostics = "No valid RHI::GraphicsAPI was selected";
                return result;
        }

        targetDesc.flags = 0;
    #ifdef OPAL_DEBUG
        targetDesc.flags |= SLANG_TARGET_FLAG_GENERATE_WHOLE_PROGRAM;
    #endif

        // Configure and Create the Compilation Session
        slang::SessionDesc sessionDesc = {};
        sessionDesc.targets = &targetDesc;
        sessionDesc.targetCount = 1;
        sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;

        Slang::ComPtr<slang::ISession> session;
        globalSession->createSession(sessionDesc, session.writeRef());

        // Load and Compile the Shader Module
        Slang::ComPtr<slang::IBlob> diagnosticsBlob;
        slang::IModule* module = session->loadModuleFromSourceString(
            "ShaderModule",
            "Shader.slang",
            source.c_str(),
            diagnosticsBlob.writeRef()
        );

        if (diagnosticsBlob && diagnosticsBlob->getBufferSize() > 0)
        {
            result.Diagnostics.assign(
                (const char*)diagnosticsBlob->getBufferPointer(),
                diagnosticsBlob->getBufferSize()
            );
        }

        if (!module)
        {
            if (!result.Diagnostics.empty())
                OPAL_ERROR("AssetManager", "Shader compile error: {}", result.Diagnostics);
            else
                result.Diagnostics = "Slang failed to compile the module without diagnostics";
            return result;
        }

        std::vector<slang::IComponentType*> components;
        components.push_back(module);


        SlangInt definedEpCount = module->getDefinedEntryPointCount();
        std::vector<Slang::ComPtr<slang::IEntryPoint>> entryPoints(definedEpCount);

        for (SlangInt32 i = 0; i < definedEpCount; i++)
        {
            module->getDefinedEntryPoint(i, entryPoints[i].writeRef());
            components.push_back(entryPoints[i].get());
        }

        Slang::ComPtr<slang::IComponentType> linkedProgram;
        session->createCompositeComponentType(
            components.data(),
            components.size(),
            linkedProgram.writeRef()
        );

        // Generate Target Code
        Slang::ComPtr<slang::IBlob> codeBlob;
        Slang::ComPtr<slang::IBlob> targetDiagnosticsBlob;

        const SlangResult targetResult = linkedProgram->getTargetCode(
            0,
            codeBlob.writeRef(),
            targetDiagnosticsBlob.writeRef()
        );

        if (targetDiagnosticsBlob && targetDiagnosticsBlob->getBufferSize() > 0)
        {
            std::string targetDiag(
                (const char*)targetDiagnosticsBlob->getBufferPointer(),
                targetDiagnosticsBlob->getBufferSize()
            );

            if (result.Diagnostics.empty())
                result.Diagnostics = targetDiag;
            else
                result.Diagnostics += "\n" + targetDiag;
        }

        if (SLANG_FAILED(targetResult) || !codeBlob)
        {
            if (!result.Diagnostics.empty())
                OPAL_ERROR("AssetManager", "Shader target generation error: {}", result.Diagnostics);
            else
                result.Diagnostics = "Slang failed to generate target bytecode";
            return result;
        }

        if (!result.Diagnostics.empty()) OPAL_WARN("AssetManager", "Shader compile diagnostics: {}", result.Diagnostics);

        result.Bytecode.resize(codeBlob->getBufferSize());
        memcpy(result.Bytecode.data(), codeBlob->getBufferPointer(), codeBlob->getBufferSize());

        if (const auto reflector = GetReflector())
        {
            result.Reflection = reflector->Reflect(linkedProgram->getLayout());
        }
        else
        {
            result.Diagnostics += result.Diagnostics.empty() ? "Shader reflection is unavailable" : "\nShader reflection is unavailable";
        }

        return result;
    }
}
