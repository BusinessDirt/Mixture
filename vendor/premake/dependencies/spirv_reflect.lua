-- vendor/premake/dependencies/spirv_reflect.lua

IncludeDir["spirv_reflect"] = "%{wks.location}/vendor/spirv-reflect"

project "SPIRV-Reflect"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "On"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "%{wks.location}/vendor/spirv-reflect/spirv_reflect.h",
        "%{wks.location}/vendor/spirv-reflect/spirv_reflect.cpp"
    }

    includedirs {
        "%{wks.location}/vendor/spriv-reflect",
        "%{wks.location}/vendor/spriv-reflect/include" -- Needed for internal spirv.h
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"
