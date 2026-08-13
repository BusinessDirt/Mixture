#if defined(OPAL_PLATFORM_DARWIN)
#include "mxpch.hpp"
#include "Platform/Metal/Pipeline/Shader.hpp"
#include "Platform/Metal/Device.hpp"

namespace Mixture::Metal
{
    Shader::Shader(Ref<Device> device, const void* data, size_t size, RHI::ShaderStage stage, RHI::ShaderIdentity identity)
        : m_Device(device), m_Stage(stage), m_Identity(identity)
    {
        if (!data || size == 0) return;

        NS::Error* error = nullptr;
        dispatch_data_t dispatchData = dispatch_data_create(data, size, nullptr, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
        m_Library = m_Device->GetHandle()->newLibrary(dispatchData, &error);
        if (error || !m_Library)
        {
            NS::String* source = NS::String::string(static_cast<const char*>(data), NS::UTF8StringEncoding);
            m_Library = m_Device->GetHandle()->newLibrary(source, nullptr, &error);
        }

        if (m_Library)
        {
            const char* entryName = (stage == RHI::ShaderStage::Vertex) ? "vertexMain" :
                                   (stage == RHI::ShaderStage::Fragment) ? "fragmentMain" : "computeMain";
            NS::String* funcName = NS::String::string(entryName, NS::UTF8StringEncoding);
            m_Function = m_Library->newFunction(funcName);
            if (!m_Function)
            {
                funcName = NS::String::string("main", NS::UTF8StringEncoding);
                m_Function = m_Library->newFunction(funcName);
            }
        }
    }

    Shader::~Shader()
    {
        if (m_Function) { m_Function->release(); m_Function = nullptr; }
        if (m_Library) { m_Library->release(); m_Library = nullptr; }
    }
}
#endif
