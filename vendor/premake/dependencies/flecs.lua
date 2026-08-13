project "Flecs"
    kind "StaticLib"
    language "C"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "%{wks.location}/vendor/flecs/src/**.c",
        "%{wks.location}/vendor/flecs/src/**.h",
        "%{wks.location}/vendor/flecs/include/**.h",
        "%{wks.location}/vendor/flecs/include/**.hpp"
    }

    includedirs {
        "%{wks.location}/vendor/flecs/include",
        "%{wks.location}/vendor/flecs/src"
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        runtime "Release"
        optimize "on"
        symbols "off"
