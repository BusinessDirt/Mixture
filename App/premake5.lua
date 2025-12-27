project "App"
    language "C++"
    cppdialect "C++20"
    targetdir "Binaries/%{cfg.buildcfg}"
    staticruntime "off"

    files { "include/**.hpp", "src/**.cpp" }
    includedirs { "include" }
    libdirs { "%{LibraryDir.vulkan}" }

    externalincludedirs {
        "../Opal/include",
        "../Mixture/include",

        "%{IncludeDir.spdlog}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.vulkan}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.spirv_reflect}",
        "%{IncludeDir.stb_image}"
    }

    links {
        "GLFW",
        "Opal",
        "Mixture",
        "ImGui",
        "SPIRV-Reflect"
    }

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    filter "configurations:Debug"
        kind "ConsoleApp"
        defines { "OPAL_DEBUG" }
        runtime "Debug"
        symbols "On"
        links {
            "%{Library.dxc_debug}",
            "%{Library.spirv_cross_debug}",
            "%{Library.spirv_cross_glsl_debug}",
            "%{Library.spirv_cross_msl_debug}"
        }

    filter "configurations:Release"
        kind "ConsoleApp"
        defines { "OPAL_RELEASE" }
        runtime "Release"
        optimize "On"
        symbols "On"
        links {
            "%{Library.dxc_release}",
            "%{Library.spirv_cross_release}",
            "%{Library.spirv_cross_glsl_release}",
            "%{Library.spirv_cross_msl_release}"
        }

    filter "configurations:Dist"
        kind "WindowedApp"
        defines { "OPAL_DIST" }
        runtime "Release"
        optimize "On"
        symbols "Off"
        links {
            "%{Library.dxc_release}",
            "%{Library.spirv_cross_release}",
            "%{Library.spirv_cross_glsl_release}",
            "%{Library.spirv_cross_msl_release}"
        }

    -- windows specific settings
    filter "system:windows"
        links { "%{Library.Vulkan}" }
        buildoptions { "/utf-8" }

    -- linux specific settings
    filter "system:linux"
        links { "vulkan" }

    -- mac specific settings
    filter "action:xcode4"
        local vulkanFW = os.getenv("VULKAN_SDK") .. "/Frameworks"
        local vulkanLibs = os.getenv("VULKAN_SDK") .. "/Lib"

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
