-- premake5.lua
include "./vendor/premake/customizations/solution_items.lua"
include "Dependencies.lua"

workspace "Mixture"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "App"
   flags { "MultiProcessorCompile" }

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
   include "vendor/premake"
   include "vendor/glfw"
   include "vendor/imgui"
group ""

group "Core"
   include "Opal/premake5.lua"
   include "Jasper/premake5.lua"
	include "Mixture/premake5.lua"
group ""

include "App/premake5.lua"