-- premake5.lua
rootdir = path.getabsolute(".")
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

local config_file = path.join(rootdir, "mixture.toml")
local config_contents = io.readfile(config_file)

if not config_contents then
    error("Could not read " .. config_file)
end

local engine_section = config_contents:match(
    "%[engine%](.-)%["
) or config_contents:match(
    "%[engine%](.*)"
)

if not engine_section then
    error("Missing [engine] section in mixture.toml")
end

engine_version = engine_section:match(
    'version%s*=%s*"([^"]+)"'
)

if not engine_version then
    error("Missing engine.version in mixture.toml")
end

version_major, version_minor, version_patch =
    engine_version:match("^(%d+)%.(%d+)%.(%d+)$")

if not version_major then
    error("engine.version must use MAJOR.MINOR.PATCH format")
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

include "Editor/premake5.lua"
