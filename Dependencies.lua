-- Dependencies
local vulkanSDK = os.getenv("VULKAN_SDK")

-- On Linux, if VULKAN_SDK is not set, try to find it in standard locations
if not vulkanSDK and os.istarget("linux") then
    if os.isdir("/usr/include/vulkan") then
        vulkanSDK = "/usr"
    end
end

IncludeDir = {}
IncludeDir["spdlog"] = "%{wks.location}/vendor/spdlog/include"
if vulkanSDK then
    IncludeDir["vulkan"] = vulkanSDK .. "/include"
else
    IncludeDir["vulkan"] = ""
end
IncludeDir["glm"] = "%{wks.location}/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/vendor/stb"
IncludeDir["flecs"] = "%{wks.location}/vendor/flecs/include"

LibraryDir = {}
if vulkanSDK then
    LibraryDir["vulkan"] = vulkanSDK .. "/lib"
else
    LibraryDir["vulkan"] = ""
end

Library = {}
if os.istarget("windows") then
    Library["vulkan"] = "vulkan-1"
else
    Library["vulkan"] = "vulkan"
end
Library["molten_vk"] = "MoltenVK"

if os.istarget("windows") then
    Library["slang_debug"] = "%{LibraryDir.vulkan}/slang.lib"
    Library["slang_release"] = "%{LibraryDir.vulkan}/slang.lib"

    Library["spirv_cross_debug"] = "%{LibraryDir.vulkan}/spirv-cross-cored.lib"
    Library["spirv_cross_glsl_debug"] = "%{LibraryDir.vulkan}/spirv-cross-glsld.lib"
    Library["spirv_cross_msl_debug"] = "%{LibraryDir.vulkan}/spirv-cross-msld.lib"

    Library["spirv_cross_release"] = "%{LibraryDir.vulkan}/spirv-cross-core.lib"
    Library["spirv_cross_glsl_release"] = "%{LibraryDir.vulkan}/spirv-cross-glsl.lib"
    Library["spirv_cross_msl_release"] = "%{LibraryDir.vulkan}/spirv-cross-msl.lib"

else
    Library["slang_debug"] = "slang"
    Library["slang_release"] = "slang"

    Library["spirv_cross_debug"] = "spirv-cross-core"
    Library["spirv_cross_glsl_debug"] = "spirv-cross-glsl"
    Library["spirv_cross_msl_debug"] = "spirv-cross-msl"

    Library["spirv_cross_release"] = "spirv-cross-core"
    Library["spirv_cross_glsl_release"] = "spirv-cross-glsl"
    Library["spirv_cross_msl_release"] = "spirv-cross-msl"
end


