project "Mixture-Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	targetdir "Binaries/%{cfg.buildcfg}"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
   	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

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

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "MX_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "MX_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "MX_DIST"
		runtime "Release"
		optimize "on"