-- premake5.lua
rootdir = path.getabsolute(".")
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

local version_file = path.join(rootdir, "VERSION")
local version_contents = io.readfile(version_file)

if not version_contents then
    error("Could not read engine version from " .. version_file)
end

version_major, version_minor, version_patch =
    version_contents:match("^%s*(%d+)%.(%d+)%.(%d+)%s*$")

if not version_major then
    error(
        "Invalid engine version in VERSION. "
        .. "Expected MAJOR.MINOR.PATCH, for example 0.1.0"
    )
end

version_major = tonumber(version_major)
version_minor = tonumber(version_minor)
version_patch = tonumber(version_patch)

engine_version = string.format(
    "%d.%d.%d",
    version_major,
    version_minor,
    version_patch
)

include "./vendor/premake/customizations/common_config.lua"
include "./vendor/premake/customizations/solution_items.lua"
include "./vendor/premake/customizations/vscode.lua"
include "Dependencies.lua"

target_architecture = _OPTIONS["arch"] or os.hostarch()

workspace "Mixture"
    architecture (target_architecture)
    configurations { "Debug", "Release", "Dist" }
    startproject "App"
    multiprocessorcompile "On"

    -- Workspace-wide build options for MSVC
    filter "system:windows"
        buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

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
