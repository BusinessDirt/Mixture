-- Dependencies
IncludeDir = {}
IncludeDir["spdlog"] = "%{wks.location}/vendor/spdlog/include"
IncludeDir["vulkan"] = os.getenv("VULKAN_SDK") .. "/include"
IncludeDir["glm"] = "%{wks.location}/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/vendor/stb"

LibraryDir = {}
LibraryDir["vulkan"] = os.getenv("VULKAN_SDK") .. "/lib"

Library = {}
Library["vulkan"] = "vulkan-1"
Library["molten_vk"] = "MoltenVK"

if os.istarget("windows") then
    Library["dxc_debug"] = "%{LibraryDir.VulkanSDK}/dxcompiler.lib"
    Library["dxc_release"] = "%{LibraryDir.VulkanSDK}/dxcompiler.lib"

    Library["spirv_cross_debug"] = "%{LibraryDir.vulkan}/spirv-cross-cored.lib"
    Library["spirv_cross_glsl_debug"] = "%{LibraryDir.vulkan}/spirv-cross-glsld.lib"
    Library["spirv_cross_msl_debug"] = "%{LibraryDir.vulkan}/spirv-cross-msld.lib"

    Library["spirv_cross_release"] = "%{LibraryDir.vulkan}/spirv-cross-core.lib"
    Library["spirv_cross_glsl_release"] = "%{LibraryDir.vulkan}/spirv-cross-glsl.lib"
    Library["spirv_cross_msl_release"] = "%{LibraryDir.vulkan}/spirv-cross-msl.lib"

else
    Library["dxc_debug"] = "dxcompiler"
    Library["dxc_release"] = "dxcompiler"

    Library["spirv_cross_debug"] = "spirv-cross-core"
    Library["spirv_cross_glsl_debug"] = "spirv-cross-glsl"
    Library["spirv_cross_msl_debug"] = "spirv-cross-msl"

    Library["spirv_cross_release"] = "spirv-cross-core"
    Library["spirv_cross_glsl_release"] = "spirv-cross-glsl"
    Library["spirv_cross_msl_release"] = "spirv-cross-msl"
end


