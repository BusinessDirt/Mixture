project "App"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "Binaries/%{cfg.buildcfg}"
    staticruntime "off"

    files { "src/**.h", "src/**.cpp" }

    includedirs {
        "src",

        -- Include Core
        "%{wks.location}/Mixture/src",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.glfw}"
    }

    links {
        "Mixture"
    }

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    filter "system:macosx"
        links {
            "GLFW",
            "Cocoa.framework",
            "IOKit.framework",
            "QuartzCore.framework"
        }

    filter "configurations:Debug"
        defines { "MX_DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "MX_RELEASE" }
        runtime "Release"
        optimize "On"
        symbols "On"

    filter "configurations:Dist"
        defines { "MX_DIST" }
        runtime "Release"
        optimize "On"
        symbols "Off"