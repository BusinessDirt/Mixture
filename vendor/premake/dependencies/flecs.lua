project "Flecs"
    kind "StaticLib"
    language "C"
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "vendor/flecs/flecs.c",
        "vendor/flecs/flecs.h"
    }

    includedirs {
        "vendor/flecs"
    }
