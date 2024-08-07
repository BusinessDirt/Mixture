project "Mixture"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    targetdir "Binaries/%{cfg.buildcfg}"
    staticruntime "off"

    pchheader "src/mxpch.hpp"
    pchsource "src/mxpch.cpp"

    files { "src/**.hpp", "src/**.cpp" }

    includedirs {
        "src",
        "%{wks.location}/Citrine/src",
    }

    externalincludedirs {
        "%{IncludeDir.glfw}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.VulkanSDK}",
    }

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    filter "system:windows"
        links {
            "GLFW",
            "%{Library.Vulkan}"
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