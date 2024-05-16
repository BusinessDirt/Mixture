project "Sandbox"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   files { "src/**.h", "src/**.cpp" }

   includedirs
   {
      "src",

	  -- Include Mixture
	  "../Mixture/src",
      "%{IncludeDir.spdlog}",
      "%{IncludeDir.ImGui}",
      "%{IncludeDir.glm}",
      "%{IncludeDir.entt}"
   }

   links
   {
      "Mixture"
   }

   targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
   objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"

   filter "system:linux"
       systemversion "latest"

   filter "system:macosx"
       systemversion "latest"

   filter "configurations:Debug"
       defines { "MX_DEBUG" }
       runtime "Debug"
       symbols "On"

       postbuildcommands {
            "{COPYDIR} \"%{LibraryDir.VulkanSDK_DebugDLL}\" \"%{cfg.targetdir}\""
       }

   filter "configurations:Release"
       defines { "MX_RELEASE" }
       runtime "Release"
       optimize "On"
       symbols "On"

   filter "configurations:Dist"
       defines { "MX_DIST" }
       runtime "Release"
       optimize "On"
       symbols "Off"