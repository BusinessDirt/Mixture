project "App"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "Binaries/%{cfg.buildcfg}"
    staticruntime "off"

    files { "src/**.hpp", "src/**.cpp" }
    libdirs { "%{LibraryDir.Vulkan}" }

    includedirs {
        "src",
        "%{wks.location}/Mixture/src"
    }

    externalincludedirs {
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.Vulkan}"
    }

    links {
        "GLFW",
        "ImGui",
        "%{Library.Vulkan}",
        "Mixture"
    }

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    -- windows specific settings
    filter "system:windows"
        links { "%{Library.Vulkan}" }
        buildoptions { "/utf-8" }

    -- mac specific settings
    filter "action:xcode4"
        links {
            "vulkan.framework",
            "Cocoa.framework",
            "Foundation.framework",
            "IOKit.framework",
            "QuartzCore.framework",
            "AppKit.framework"
        }

        frameworkdirs {
            "/System/Library/Frameworks",
            "%{LibraryDir.Vulkan}",
            "%{LibraryDir.VulkanFrameworks}"
        }

        xcodebuildsettings {
            ["LD_RUNPATH_SEARCH_PATHS"] = "@executable_path/../Frameworks @loader_path/../Frameworks %{LibraryDir.VulkanFrameworks} %{LibraryDir.Vulkan}"
        }

    filter "configurations:Debug"
        kind "ConsoleApp"
        defines { "OPAL_DEBUG" }
        runtime "Debug"
        symbols "On"
        links { "%{Library.ShaderC_Debug}", "%{Library.SPIRV_Cross_Debug}" }

    filter "configurations:Release"
        kind "ConsoleApp"
        defines { "OPAL_RELEASE" }
        runtime "Release"
        optimize "On"
        symbols "On"
        links { "%{Library.ShaderC_Release}", "%{Library.SPIRV_Cross_Release}" }

    filter "configurations:Dist"
        kind "WindowedApp"
        defines { "OPAL_DIST" }
        runtime "Release"
        optimize "On"
        symbols "Off"
        links { "%{Library.ShaderC_Release}", "%{Library.SPIRV_Cross_Release}" }
