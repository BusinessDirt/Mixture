project "Tests"
    kind "ConsoleApp"
    common_language_spec()
    common_target()
    common_directories()

    externalincludedirs {
        application_externalincludedirs(),
        "%{IncludeDir.googletest}"
    }

    links {
        application_links(),
        "GoogleTest"
    }

    filter "configurations:Debug"
        application_debug_settings()

    filter "configurations:Release"
        application_release_settings()

    filter "configurations:Dist"
        application_dist_settings()

    filter "system:windows"
        windows_settings()

    filter "system:linux"
        linux_settings()

    filter "action:xcode4"
        xcode_settings()

    filter {}
