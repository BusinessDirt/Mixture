-- premake5.lua
include "./vendor/premake/customizations/vscode.lua"
include "./vendor/premake/customizations/solution_items.lua"
include "Dependencies.lua"

workspace "Mixture"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }
    startproject "App"
    multiprocessorcompile ("on")

    postbuildcommands {
        "{COPY} %{wks.location}/Assets %{cfg.targetdir}"
    }

    -- Workspace-wide build options for MSVC
    filter "system:windows"
        buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
    include "vendor/premake/dependencies/glfw.lua"
    include "vendor/premake/dependencies/imgui.lua"
    include "vendor/premake/dependencies/googletest.lua"
    include "vendor/premake/dependencies/spirv_reflect.lua"
group ""

group "Core"
    include "Opal/premake5.lua"
	include "Mixture/premake5.lua"
group ""

group "Test"
    include "Tests/premake5.lua"
group ""

include "App/premake5.lua"
