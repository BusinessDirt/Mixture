project "App"
    language "C++"
    cppdialect "C++20"
    targetdir "Binaries/%{cfg.buildcfg}"
    staticruntime "off"

    files { "include/**.hpp", "src/**.cpp" }
    includedirs { "include" }
    libdirs { "%{LibraryDir.Vulkan}" }

    externalincludedirs {
        "../Opal/include",
        "../Jasper/include",
        "../Mixture/include",

        "%{IncludeDir.spdlog}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.Vulkan}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}"
    }

    links {
        "GLFW",
        "Opal",
        "Jasper",
        "Mixture",
        "ImGui"
    }

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    filter "configurations:Debug"
        kind "ConsoleApp"
        defines { "OPAL_DEBUG" }
        runtime "Debug"
        symbols "On"
        links {
            "%{Library.ShaderC_Debug}",
            "%{Library.SPIRV_Cross_Debug}"
        }

    filter "configurations:Release"
        kind "ConsoleApp"
        defines { "OPAL_RELEASE" }
        runtime "Release"
        optimize "On"
        symbols "On"
        links {
            "%{Library.ShaderC_Release}",
            "%{Library.SPIRV_Cross_Release}"
        }

    filter "configurations:Dist"
        kind "WindowedApp"
        defines { "OPAL_DIST" }
        runtime "Release"
        optimize "On"
        symbols "Off"
        links {
            "%{Library.ShaderC_Release}",
            "%{Library.SPIRV_Cross_Release}"
        }   

    -- windows specific settings
    filter "system:windows"
        links { "%{Library.Vulkan}" }
        buildoptions { "/utf-8" }

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
            "%{LibraryDir.Vulkan}"
        }

        xcodebuildsettings {
            ["LD_RUNPATH_SEARCH_PATHS"] = "@executable_path/../Frameworks @loader_path/../Frameworks " .. vulkanFW .. " " .. vulkanLibs
        }