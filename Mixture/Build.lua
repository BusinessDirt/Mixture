project "Mixture"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    targetdir "Binaries/%{cfg.buildcfg}"
    staticruntime "off"

    pchsource "src/mxpch.cpp"

    files { "src/**.hpp", "src/**.cpp" }

    includedirs {
        "src"
    }

    externalincludedirs {
        "%{IncludeDir.glfw}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.VulkanSDK}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.tinyobjloader}"
    }

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    filter "system:windows"
        links {
            "GLFW",
            "ImGui",
            "%{Library.Vulkan}"
        }

        pchheader "mxpch.hpp"

    filter "system:macosx"
        pchheader "src/mxpch.hpp"

    filter "configurations:Debug"
        defines { "MX_DEBUG" }
        runtime "Debug"
        symbols "On"
        filter "system:windows"
            links {
                "%{Library.ShaderC_Debug}",
                "%{Library.SPIRV_Cross_Debug}"
            }

    filter "configurations:Release"
        defines { "MX_RELEASE" }
        runtime "Release"
        optimize "On"
        symbols "On"
        filter "system:windows"
            links {
                "%{Library.ShaderC_Release}",
                "%{Library.SPIRV_Cross_Release}"
            }

    filter "configurations:Dist"
        defines { "MX_DIST" }
        runtime "Release"
        optimize "On"
        symbols "Off"
        filter "system:windows"
            links {
                "%{Library.ShaderC_Release}",
                "%{Library.SPIRV_Cross_Release}"
            }