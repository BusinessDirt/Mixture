project "Tests"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    files { "include/**.hpp", "src/**.cpp" }
    includedirs { "include" }

    externalincludedirs {
        "../Mixture/include",
        "%{IncludeDir.googletest}"
    }

    links {
        "GLFW",
        "Opal",
        "Mixture",
        "ImGui",
        "GoogleTest"
    }

    -- Auto-run tests after build (Optional but recommended)
    postbuildcommands {
        "{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Tests", -- Move binary if needed
        --On Windows, might want to run it:
        -- "%{cfg.buildtarget.relpath}"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        runtime "Release"
        optimize "On"