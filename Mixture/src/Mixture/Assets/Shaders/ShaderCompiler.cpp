#include "mxpch.hpp"
#include "Mixture/Assets/Shaders/ShaderCompiler.hpp"
#include "Mixture/Assets/Shaders/IShaderReflector.hpp"

#include "Mixture/Assets/AssetManager.hpp"

#ifdef _WIN32
    #include <Windows.h>
    #include <atlbase.h>
#else
    #include <dxc/WinAdapter.h>
#endif
#include <dxc/dxcapi.h>
#include <spirv_reflect.h>

namespace Mixture
{
    namespace
    {
        struct CompilerState
        {
            CComPtr<IDxcUtils> Utils;
            CComPtr<IDxcCompiler3> Compiler;
            bool Available = false;

            CompilerState()
            {
                const HRESULT utilsResult = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&Utils));
                const HRESULT compilerResult = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&Compiler));
                Available = SUCCEEDED(utilsResult) && SUCCEEDED(compilerResult) && Utils && Compiler;
            }
        };

        CompilerState& GetCompilerState()
        {
            static CompilerState state;
            return state;
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
            result.Diagnostics = "Shader source exceeds the DXC input-size limit";
            return result;
        }
        CompilerState& state = GetCompilerState();
        if (!state.Available)
        {
            OPAL_ERROR("AssetManager", "DXC is unavailable; shader compilation cannot continue.");
            result.Diagnostics = "DXC is unavailable";
            return result;
        }

        // Create a blob from the source string
        CComPtr<IDxcBlobEncoding> pSource;
        const HRESULT blobResult = state.Utils->CreateBlob(
            source.c_str(), static_cast<uint32_t>(source.length()), CP_UTF8, &pSource);
        if (FAILED(blobResult) || !pSource)
        {
            result.Diagnostics = "DXC failed to create a source blob";
            return result;
        }

        // Set up Compiler Arguments
        Vector<LPCWSTR> arguments;

        // Use Library profile (lib_6_x) to support multiple undefined entry points
        arguments.push_back(L"-T");
        arguments.push_back(L"lib_6_5"); // Shader Model 6.5

#ifdef OPAL_DEBUG
        arguments.push_back(L"-Od");
        arguments.push_back(L"-Zi");
        arguments.push_back(L"-Qembed_debug");
#else
        arguments.push_back(L"-O3");
#endif

        // API-Specific Flags
        if (graphicsAPI == RHI::GraphicsAPI::Vulkan || graphicsAPI == RHI::GraphicsAPI::Metal)
        {
            // Both Vulkan and Metal start with SPIR-V
            arguments.push_back(L"-spirv");
            arguments.push_back(L"-fspv-target-env=vulkan1.3");

            // Important: Preserve bindings so SPIRV-Cross can reflect them later
            arguments.push_back(L"-fvk-use-dx-layout");
        }

        // Run Compilation
        DxcBuffer sourceBuffer;
        sourceBuffer.Ptr = pSource->GetBufferPointer();
        sourceBuffer.Size = pSource->GetBufferSize();
        sourceBuffer.Encoding = DXC_CP_UTF8;

        CComPtr<IDxcResult> pResults;
        const HRESULT compileResult = state.Compiler->Compile(&sourceBuffer, arguments.data(),
            static_cast<uint32_t>(arguments.size()), nullptr, IID_PPV_ARGS(&pResults));
        if (FAILED(compileResult) || !pResults)
        {
            result.Diagnostics = "DXC failed to start compilation";
            return result;
        }

        // Check for Errors
        CComPtr<IDxcBlobUtf8> pErrors;
        const HRESULT errorsResult = pResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrors), nullptr);
        if (FAILED(errorsResult))
        {
            result.Diagnostics = "DXC failed to retrieve compiler diagnostics";
            return result;
        }
        if (pErrors && pErrors->GetStringLength() != 0)
            result.Diagnostics.assign(pErrors->GetStringPointer(), pErrors->GetStringLength());

        HRESULT compilationStatus = E_FAIL;
        const HRESULT statusResult = pResults->GetStatus(&compilationStatus);
        if (FAILED(statusResult) || FAILED(compilationStatus))
        {
            if (!result.Diagnostics.empty())
                OPAL_ERROR("AssetManager", "Shader compile error: {}", result.Diagnostics);
            else
                result.Diagnostics = "DXC compilation failed without diagnostics";
            return result;
        }

        if (!result.Diagnostics.empty())
            OPAL_WARN("AssetManager", "Shader compile diagnostics: {}", result.Diagnostics);

        // Get the Output Blob (DXIL or SPIR-V)
        CComPtr<IDxcBlob> pShaderBlob;
        const HRESULT objectResult = pResults->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShaderBlob), nullptr);
        if (FAILED(objectResult) || !pShaderBlob || pShaderBlob->GetBufferSize() == 0)
        {
            result.Diagnostics = "DXC reported success but returned no shader object";
            return result;
        }

        result.Bytecode.resize(pShaderBlob->GetBufferSize());
        memcpy(result.Bytecode.data(), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize());
        return result;
    }
}
