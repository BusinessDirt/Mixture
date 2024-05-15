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
		"%{wks.location}/vendor/stb_image/**.cpp",

        "%{wks.location}/vendor/imguizmo/ImGuizmo.h",
        "%{wks.location}/vendor/imguizmo/ImGuizmo.cpp"
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
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.yaml_cpp}",
        "%{IncludeDir.ImGuizmo}"
    }

    links {
            "GLFW",
            "Glad",
            "ImGui",
            "yaml-cpp",
            "opengl32.lib"
    }

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    filter "files:%{wks.location}/vendor/imguizmo/**.cpp"
        flags { "NoPCH" }

    filter "system:windows"
        systemversion "latest"

    filter "system:linux"
        systemversion "latest"

    filter "system:macosx"
        systemversion "latest"

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