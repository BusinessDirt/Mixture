-- premake5.lua
include "./vendor/premake/customizations/solution_items.lua"
include "Dependencies.lua"

workspace "New Project"
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
group ""

group "Core"
	include "Core/premake5.lua"
group ""

include "App/premake5.lua"