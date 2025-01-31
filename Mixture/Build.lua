project "Mixture"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    targetdir "Binaries/%{cfg.buildcfg}"
    staticruntime "off"

    pchsource "src/mxpch.cpp"

    files { "src/**.hpp", "src/**.cpp" }

    includedirs { "src" }

    externalincludedirs {
        "%{IncludeDir.glfw}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.Vulkan}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.tinyobjloader}"
    }

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    filter "action:xcode4"
        pchheader "src/mxpch.hpp"
        filter "files:**.mm"
            compileas "Objective-C++"  -- Compile .mm files as Objective-C++

    -- Add the /utf-8 flag
    filter "action:vs2022" -- Only apply for MSVC toolset
        buildoptions { "/utf-8" }
        pchheader "mxpch.hpp"

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