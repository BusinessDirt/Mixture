#include "mxpch.hpp"
#include "Mixture/Assets/Shaders/ShaderCompiler.hpp"

#include "Mixture/Assets/AssetManager.hpp"

#include <dxc/dxcapi.h>
#include <spirv_cross/spirv_msl.hpp>

// --- CROSS-PLATFORM SMART POINTER ---
// This replaces Microsoft::WRL::ComPtr so the code works on Linux/Mac/Win
template <typename T>
class DxcPtr
{
public:
    T* ptr = nullptr;

    DxcPtr() = default;
    ~DxcPtr() { Release(); }

    // No copy allowed (simplifies ownership)
    DxcPtr(const DxcPtr&) = delete;
    DxcPtr& operator=(const DxcPtr&) = delete;

    // Address-of operator (allows passing &ptr to DxcCreateInstance)
    T** operator&() { Release(); return &ptr; }

    // Access operators
    T* operator->() { return ptr; }
    T* Get() { return ptr; }

    // Boolean check
    explicit operator bool() const { return ptr != nullptr; }

    void Release()
    {
        if (ptr)
        {
            ptr->Release();
            ptr = nullptr;
        }
    }
};

// Helper for IID_PPV_ARGS on non-Windows
#ifndef OPAL_PLATFORM_WINDOWS
    #ifndef IID_PPV_ARGS
        // Linux/Mac GUID helper
        #define IID_PPV_ARGS(ppType) __uuidof(**(ppType)), (void**)(ppType)
    #endif
#endif

namespace Mixture
{
    Vector<uint8_t> ShaderCompiler::Compile(const std::string& source)
        {
            auto graphicsAPI = AssetManager::Get().GetGraphicsAPI();

            // These are initialized only ONCE the first time Compile is called.
            // C++11 guarantees this initialization is thread-safe.
            static DxcPtr<IDxcUtils> pUtils;
            static DxcPtr<IDxcCompiler3> pCompiler;
            static bool bInitialized = [&]() {
                DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&pUtils));
                DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&pCompiler));
                return true;
            }();

            // Create a blob from the source string
            DxcPtr<IDxcBlobEncoding> pSource;
            pUtils->CreateBlob(source.c_str(), source.length(), CP_UTF8, &pSource);

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
            else if (graphicsAPI == RHI::GraphicsAPI::D3D12)
            {
                // Embed debug info or strip it depending on needs (Stripping recommended for cache)
                arguments.push_back(L"-Qstrip_debug");
                arguments.push_back(L"-Qstrip_reflect");
            }

            // Run Compilation
            DxcBuffer sourceBuffer;
            sourceBuffer.Ptr = pSource->GetBufferPointer();
            sourceBuffer.Size = pSource->GetBufferSize();
            sourceBuffer.Encoding = DXC_CP_UTF8;

            DxcPtr<IDxcResult> pResults;
            pCompiler->Compile(&sourceBuffer, arguments.data(), (uint32_t)arguments.size(), nullptr, IID_PPV_ARGS(&pResults));

            // Check for Errors
            DxcPtr<IDxcBlobUtf8> pErrors;
            pResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrors), nullptr);
            if (pErrors && pErrors->GetStringLength() != 0)
            {
                OPAL_ERROR("Core/Assets", "Shader Compile Errror: {}", pErrors->GetStringPointer());
                return {};
            }

            // Get the Output Blob (DXIL or SPIR-V)
            DxcPtr<IDxcBlob> pShaderBlob;
            pResults->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShaderBlob), nullptr);

            if (!pShaderBlob) return {};

            // Case 1: D3D12 (DXIL) & Vulkan (SPIR-V)
            // These APIs accept the binary exactly as DXC outputs it.
            if (graphicsAPI == RHI::GraphicsAPI::D3D12 || graphicsAPI == RHI::GraphicsAPI::Vulkan)
            {
                Vector<uint8_t> nativeBlob(pShaderBlob->GetBufferSize());
                memcpy(nativeBlob.data(), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize());
                return nativeBlob;
            }

            // Case 2: Metal (SPIR-V -> MSL)
            // Metal needs Source Code (text) to compile at runtime.
            // -> Convert the SPIR-V to MSL text, then store that text as bytes.
            if (graphicsAPI == RHI::GraphicsAPI::Metal)
            {
                const uint32_t* spirvData = reinterpret_cast<const uint32_t*>(pShaderBlob->GetBufferPointer());
                size_t wordCount = pShaderBlob->GetBufferSize() / sizeof(uint32_t);

                spirv_cross::CompilerMSL mslCompiler(spirvData, wordCount);
                spirv_cross::CompilerMSL::Options mslOptions;
                mslOptions.set_msl_version(2, 3);
                mslOptions.argument_buffers_tier = spirv_cross::CompilerMSL::Options::ArgumentBuffersTier::Tier2;
                mslCompiler.set_msl_options(mslOptions);

                std::string mslSource = mslCompiler.compile();
                return Vector<uint8_t>(mslSource.begin(), mslSource.end());
            }

            return {};
        }
}
