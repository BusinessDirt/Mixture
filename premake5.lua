-- premake5.lua
include "./vendor/premake/customizations/solution_items.lua"
include "Dependencies.lua"

workspace "Mixture"
   architecture "x86_64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Sandbox"
   flags { "MultiProcessorCompile" }

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
   include "vendor/premake"
   include "vendor/glfw"
   include "vendor/glad"
   include "vendor/imgui"
   include "vendor/yaml-cpp"
group ""


include "Mixture/premake5.lua"
include "Mixture-Editor/premake5.lua"
include "Sandbox/premake5.lua"