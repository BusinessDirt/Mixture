-- premake5.lua
include "./vendor/premake/customizations/vscode.lua"
include "./vendor/premake/customizations/solution_items.lua"
include "Dependencies.lua"

local root_dir = path.getabsolute(".")

workspace "Mixture"
    architecture "x64"
    configurations { "Debug", "Release", "Dist" }
    startproject "App"
    multiprocessorcompile ("on")

    -- Workspace-wide build options for MSVC
    filter "system:windows"
        buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

    filter "action:xcode4"
        postbuildcommands {
            "rm -rf \"%{cfg.targetdir}/Assets\"",
            "ln -sf \"" .. root_dir .. "/Assets\" \"%{cfg.targetdir}/Assets\""
        }
    filter {}

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
