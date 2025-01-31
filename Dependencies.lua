-- Dependencies
VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["Vulkan"] = "%{VULKAN_SDK}/include"
IncludeDir["glm"] = "%{wks.location}/vendor/glm"
IncludeDir["glfw"] = "%{wks.location}/vendor/glfw/include"
IncludeDir["spdlog"] = "%{wks.location}/vendor/spdlog/include"
IncludeDir["stb"] = "%{wks.location}/vendor/stb"
IncludeDir["imgui"] = "%{wks.location}/vendor/imgui"
IncludeDir["tinyobjloader"] = "%{wks.location}/vendor/tinyobjloader"

LibraryDir = {}
LibraryDir["Vulkan"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanFrameworks"] = "%{VULKAN_SDK}/Frameworks"

Library = {}
Library["Vulkan"] = "vulkan-1"
Library["MoltenVK"] = "MoltenVK"

if os.istarget("windows") then
    Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
    Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
    Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
    Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

    Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
    Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
    Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"
elseif os.istarget("linux") then

elseif os.istarget("macosx") then
    Library["ShaderC_Debug"] = "shaderc_shared"
    Library["SPIRV_Cross_Debug"] = "spirv-cross-core"
    Library["SPIRV_Cross_GLSL_Debug"] = "spirv-cross-glsl"

    Library["ShaderC_Release"] = "shaderc_shared"
    Library["SPIRV_Cross_Release"] = "spirv-cross-glsl"
    Library["SPIRV_Cross_GLSL_Release"] = "spirv-cross-glsl"
end
