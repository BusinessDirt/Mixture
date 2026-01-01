-- Dependencies
local vulkanSDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["spdlog"] = "%{wks.location}/vendor/spdlog/include"
if vulkanSDK then
    IncludeDir["vulkan"] = os.getenv("VULKAN_SDK") .. "/include"
else
    IncludeDir["vulkan"] = ""
end
IncludeDir["glm"] = "%{wks.location}/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/vendor/stb"

LibraryDir = {}
if vulkanSDK then
    LibraryDir["vulkan"] = os.getenv("VULKAN_SDK") .. "/lib"
else
    LibraryDir["vulkan"] = ""
end

Library = {}
Library["vulkan"] = "vulkan-1"
Library["molten_vk"] = "MoltenVK"

if os.istarget("windows") then
    Library["dxc_debug"] = "%{LibraryDir.vulkan}/dxcompiler.lib"
    Library["dxc_release"] = "%{LibraryDir.vulkan}/dxcompiler.lib"

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


