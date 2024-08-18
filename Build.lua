-- premake5.lua
include "./vendor/premake/customizations/solution_items.lua"
include "Dependencies.lua"

workspace "Mixture"
   configurations { "Debug", "Release", "Dist" }
   startproject "App"
   flags { "MultiProcessorCompile" }

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }
      architecture "x64"

   filter "system:macosx"
      architecture "ARM64"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
   include "vendor/premake"
   include "vendor/glfw"
   include "vendor/imgui"
group ""

group "Core"
	include "Mixture/Build.lua"
group ""

include "App/Build.lua"