-- Dependencies
IncludeDir = {}
IncludeDir["spdlog"] = "%{wks.location}/vendor/spdlog/include"
IncludeDir["Vulkan"] = os.getenv("VULKAN_SDK") .. "/include"
IncludeDir["glm"] = "%{wks.location}/vendor/glm"
IncludeDir["glfw"] = "%{wks.location}/vendor/glfw/include"
IncludeDir["stb_image"] = "%{wks.location}/vendor/stb"
IncludeDir["imgui"] = "%{wks.location}/vendor/imgui"
IncludeDir["googletest"] = "%{wks.location}/vendor/googletest/googletest/include"

LibraryDir = {}
LibraryDir["Vulkan"] = os.getenv("VULKAN_SDK") .. "/lib"

Library = {}
Library["Vulkan"] = "vulkan-1"
Library["MoltenVK"] = "MoltenVK"

if os.istarget("windows") then
    Library["DXC_Debug"] = "%{LibraryDir.VulkanSDK}/dxcompiler.lib"
    Library["DXC_Release"] = "%{LibraryDir.VulkanSDK}/dxcompiler.lib"

    Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
    Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
    Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

    Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
    Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

elseif os.istarget("macosx") then
    Library["DXC_Debug"] = "dxcompiler"
    Library["DXC_Release"] = "dxcompiler"

    Library["SPIRV_Cross_Debug"] = "spirv-cross-core"
    Library["SPIRV_Cross_GLSL_Debug"] = "spirv-cross-glsl"

    Library["SPIRV_Cross_Release"] = "spirv-cross-core"
    Library["SPIRV_Cross_GLSL_Release"] = "spirv-cross-glsl"

elseif os.istarget("linux") then
    Library["DXC_Debug"] = "dxcompiler"
    Library["DXC_Release"] = "dxcompiler"

    Library["SPIRV_Cross_Debug"] = "spirv-cross-core"
    Library["SPIRV_Cross_GLSL_Debug"] = "spirv-cross-glsl"

    Library["SPIRV_Cross_Release"] = "spirv-cross-core"
    Library["SPIRV_Cross_GLSL_Release"] = "spirv-cross-glsl"
end


