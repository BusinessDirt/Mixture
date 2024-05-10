project "Mixture"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    targetdir "Binaries/%{cfg.buildcfg}"
    staticruntime "off"

    pchheader "mxpch.h"
    pchsource "src/mxpch.cpp"

    files { 
        "src/**.h",
		"src/**.cpp",
		"%{wks.location}/vendor/stb_image/**.h",
		"%{wks.location}/vendor/stb_image/**.cpp"
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE"
    }

    includedirs {
        "src",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}"
    }

    links {
            "GLFW",
            "Glad",
            "ImGui",
            "opengl32.lib"
    }

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    filter "system:windows"
        systemversion "latest"
        defines { "MIXTURE_PLATFORM_WINDOWS" }

    filter "system:linux"
        systemversion "latest"
        defines { "MIXTURE_PLATFORM_LINUX" }

    filter "system:macosx"
        systemversion "latest"
        defines { "MIXTURE_PLATFORM_MACOSX" }

    filter "configurations:Debug"
        defines { "MIXTURE_DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "MIXTURE_RELEASE" }
        runtime "Release"
        optimize "On"
        symbols "On"

    filter "configurations:Dist"
        defines { "MIXTURE_DIST" }
        runtime "Release"
        optimize "On"
        symbols "Off"