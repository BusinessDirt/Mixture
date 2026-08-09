-- Sets Language and version
function common_language_spec()
    language "C++"
    cppdialect "C++20"
end

function common_target()
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
    debugdir "%{wks.location}"
end

-- files, includedirs (./include), libdirs (vulkan)
function common_directories()
    files { "include/**.hpp", "src/**.cpp" }
    includedirs { "include" }
    libdirs { "%{LibraryDir.vulkan}" }
end

function application_externalincludedirs()
    return {
        "../Opal/include",
        "../Mixture/include",

        "%{IncludeDir.spdlog}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.vulkan}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.spirv_reflect}",
        "%{IncludeDir.stb_image}"
    }
end

function application_links()
    return {
        "Mixture",
        "ImGui",
        "SPIRV-Reflect",
        "GLFW",
        "Opal"
    }
end

function application_debug_links()
    return {
        "%{Library.dxc_debug}",
        "%{Library.spirv_cross_debug}",
        "%{Library.spirv_cross_glsl_debug}",
        "%{Library.spirv_cross_msl_debug}"
    }
end

function application_ndebug_links()
    return {
        "%{Library.dxc_release}",
        "%{Library.spirv_cross_release}",
        "%{Library.spirv_cross_glsl_release}",
        "%{Library.spirv_cross_msl_release}"
    }
end

function application_debug_settings()
    defines { "OPAL_DEBUG" }
    runtime "Debug"
    symbols "On"
    links { application_debug_links() }
end

function application_release_settings()
    defines { "OPAL_RELEASE" }
    runtime "Release"
    optimize "On"
    symbols "On"
    links { application_ndebug_links() }
end

function application_dist_settings()
    defines { "OPAL_DIST" }
    runtime "Release"
    optimize "On"
    symbols "Off"
    links { application_ndebug_links() }
end

function windows_settings()
    links { "%{Library.vulkan}" }
    buildoptions { "/utf-8" }
end

function linux_settings()
    links { "%{Library.vulkan}" }
end

function xcode_settings()
    local vulkanSDK = os.getenv("VULKAN_SDK") or ""
    local vulkanFW = vulkanSDK .. "/Frameworks"
    local vulkanLibs = vulkanSDK .. "/Lib"

    links {
        "vulkan.framework",
        "Cocoa.framework",
        "Foundation.framework",
        "IOKit.framework",
        "QuartzCore.framework",
        "AppKit.framework"
    }

    frameworkdirs {
        vulkanFW,
        "/System/Library/Frameworks",
        "%{LibraryDir.vulkan}"
    }

    xcodebuildsettings {
        ["LD_RUNPATH_SEARCH_PATHS"] = "@executable_path/../Frameworks @loader_path/../Frameworks " .. vulkanFW .. " " .. vulkanLibs
    }

    buildoutputs { "%{cfg.targetdir}/Assets" }
    postbuildcommands {
        "rm -rf \"%{cfg.targetdir}/Assets\"",
        "ln -sf \"" .. rootdir .. "/Assets\" \"%{cfg.targetdir}/Assets\""
    }

end
