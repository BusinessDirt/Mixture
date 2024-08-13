-- Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/include"
IncludeDir["glm"] = "%{wks.location}/vendor/glm"
IncludeDir["glfw"] = "%{wks.location}/vendor/glfw/include"
IncludeDir["spdlog"] = "%{wks.location}/vendor/spdlog/include"
IncludeDir["stb"] = "%{wks.location}/vendor/stb"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/lib"

Library = {}

filter "system:windows"
    Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
    Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
    Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
    Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
    Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

    Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
    Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
    Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

filter "action:xcode4"
    Library["Vulkan"] = "%{VULKAN_SDK}/Frameworks"
