-- vendor/premake/dependencies/glfw.lua

IncludeDir["glfw"] = "%{wks.location}/vendor/glfw/include"

project "GLFW"
	kind "StaticLib"
	language "C"
	staticruntime "off"
	warnings "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{wks.location}/vendor/glfw/include/GLFW/glfw3.h",
		"%{wks.location}/vendor/glfw/include/GLFW/glfw3native.h",
		"%{wks.location}/vendor/glfw/src/glfw_config.h",
		"%{wks.location}/vendor/glfw/src/context.c",
		"%{wks.location}/vendor/glfw/src/init.c",
		"%{wks.location}/vendor/glfw/src/input.c",
		"%{wks.location}/vendor/glfw/src/monitor.c",

		"%{wks.location}/vendor/glfw/src/null_init.c",
		"%{wks.location}/vendor/glfw/src/null_joystick.c",
		"%{wks.location}/vendor/glfw/src/null_monitor.c",
		"%{wks.location}/vendor/glfw/src/null_window.c",

		"%{wks.location}/vendor/glfw/src/platform.c",
		"%{wks.location}/vendor/glfw/src/vulkan.c",
		"%{wks.location}/vendor/glfw/src/window.c",
	}

	filter "system:linux"
		pic "On"

		systemversion "latest"

		files
		{
			"%{wks.location}/vendor/glfw/src/x11_init.c",
			"%{wks.location}/vendor/glfw/src/x11_monitor.c",
			"%{wks.location}/vendor/glfw/src/x11_window.c",
			"%{wks.location}/vendor/glfw/src/xkb_unicode.c",
			"%{wks.location}/vendor/glfw/src/posix_module.c",
			"%{wks.location}/vendor/glfw/src/posix_time.c",
			"%{wks.location}/vendor/glfw/src/posix_thread.c",
			"%{wks.location}/vendor/glfw/src/posix_module.c",
			"%{wks.location}/vendor/glfw/src/glx_context.c",
			"%{wks.location}/vendor/glfw/src/egl_context.c",
			"%{wks.location}/vendor/glfw/src/osmesa_context.c",
			"%{wks.location}/vendor/glfw/src/linux_joystick.c"
		}

		defines
		{
			"_GLFW_X11"
		}

	filter "system:macosx"
		pic "On"

		files
		{
			"%{wks.location}/vendor/glfw/src/cocoa_init.m",
			"%{wks.location}/vendor/glfw/src/cocoa_monitor.m",
			"%{wks.location}/vendor/glfw/src/cocoa_window.m",
			"%{wks.location}/vendor/glfw/src/cocoa_joystick.m",
			"%{wks.location}/vendor/glfw/src/cocoa_time.c",
			"%{wks.location}/vendor/glfw/src/nsgl_context.m",
			"%{wks.location}/vendor/glfw/src/posix_thread.c",
			"%{wks.location}/vendor/glfw/src/posix_module.c",
			"%{wks.location}/vendor/glfw/src/osmesa_context.c",
			"%{wks.location}/vendor/glfw/src/egl_context.c"
		}

		defines
		{
			"_GLFW_COCOA"
		}

	filter "system:windows"
		systemversion "latest"

		files
		{
			"%{wks.location}/vendor/glfw/src/win32_init.c",
			"%{wks.location}/vendor/glfw/src/win32_joystick.c",
			"%{wks.location}/vendor/glfw/src/win32_module.c",
			"%{wks.location}/vendor/glfw/src/win32_monitor.c",
			"%{wks.location}/vendor/glfw/src/win32_time.c",
			"%{wks.location}/vendor/glfw/src/win32_thread.c",
			"%{wks.location}/vendor/glfw/src/win32_window.c",
			"%{wks.location}/vendor/glfw/src/wgl_context.c",
			"%{wks.location}/vendor/glfw/src/egl_context.c",
			"%{wks.location}/vendor/glfw/src/osmesa_context.c"
		}

		defines
		{
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "speed"

    filter "configurations:Dist"
		runtime "Release"
		optimize "speed"
        symbols "off"
