#if defined(OPAL_PLATFORM_DARWIN)
#include "mxpch.hpp"
#include "Platform/Metal/Pipeline/Shader.hpp"
#include "Platform/Metal/Device.hpp"

namespace Mixture::Metal
{
    Shader::Shader(Ref<Device> device, const void* data, size_t size, RHI::ShaderStage stage,
                   RHI::ShaderIdentity identity, const ShaderReflectionData& reflection)
        : m_Device(device), m_Stage(stage), m_ReflectionData(&reflection), m_Identity(identity)
    {
        if (!m_Device || !data || size == 0)
        {
            OPAL_ERROR("Core/Metal", "Cannot create a Metal shader from empty data or without a device.");
            return;
        }

        NS::Error* error = nullptr;
        dispatch_data_t dispatchData = dispatch_data_create(data, size, nullptr, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
        m_Library = m_Device->GetHandle()->newLibrary(dispatchData, &error);
        if (error || !m_Library)
        {
            if (error) OPAL_LOG_DEBUG("Core/Metal", "Metal binary library load failed; trying source: {}", error->localizedDescription()->utf8String());

            std::string safeSource((const char*)data, size);
            NS::String* nsSource = NS::String::string(safeSource.c_str(), NS::UTF8StringEncoding);

            m_Library = m_Device->GetHandle()->newLibrary(nsSource, nullptr, &error);
        }

        if (!m_Library)
        {
            OPAL_ERROR("Core/Metal", "Failed to create Metal shader library: {}",
                error ? error->localizedDescription()->utf8String() : "unknown error");
            return;
        }

        const auto entryPoint = reflection.EntryPoints.find(stage);
        const char* entryName = entryPoint != reflection.EntryPoints.end() ? entryPoint->second.c_str() : "main";
        m_Function = m_Library->newFunction(NS::String::string(entryName, NS::UTF8StringEncoding));
        if (!m_Function && std::string_view(entryName) != "main")
            m_Function = m_Library->newFunction(NS::String::string("main", NS::UTF8StringEncoding));
        if (!m_Function)
            OPAL_ERROR("Core/Metal", "Failed to find '{}' entry point in Metal shader library.", entryName);
        else
            OPAL_LOG_DEBUG("Core/Metal", "Created Metal {} shader using '{}'.", static_cast<uint32_t>(stage), entryName);
    }

    Shader::~Shader()
    {
        if (m_Function) { m_Function->release(); m_Function = nullptr; }
        if (m_Library) { m_Library->release(); m_Library = nullptr; }
    }
}
#endif
