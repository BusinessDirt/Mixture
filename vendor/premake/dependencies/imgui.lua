-- vendor/premake/dependencies/imgui.lua

IncludeDir["imgui"] = "%{wks.location}/vendor/imgui"

project "ImGui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "%{wks.location}/vendor/imgui/imconfig.h",
        "%{wks.location}/vendor/imgui/imgui.h",
        "%{wks.location}/vendor/imgui/imgui.cpp",
        "%{wks.location}/vendor/imgui/imgui_draw.cpp",
        "%{wks.location}/vendor/imgui/imgui_internal.h",
        "%{wks.location}/vendor/imgui/imgui_tables.cpp",
        "%{wks.location}/vendor/imgui/imgui_widgets.cpp",
        "%{wks.location}/vendor/imgui/imstb_rectpack.h",
        "%{wks.location}/vendor/imgui/imstb_textedit.h",
        "%{wks.location}/vendor/imgui/imstb_truetype.h",
        "%{wks.location}/vendor/imgui/imgui_demo.cpp"
    }

    filter "system:linux"
  	    pic "On"

    filter { "toolset:msc*" }
        files { "%{wks.location}/vendor/imgui/misc/debuggers/*.natvis" }

    filter "configurations:Debug"
        runtime "Debug"
  	    symbols "on"

    filter "configurations:Release"
  	    runtime "Release"
  	    optimize "on"

    filter "configurations:Dist"
  	    runtime "Release"
  	    optimize "on"
        symbols "off"
