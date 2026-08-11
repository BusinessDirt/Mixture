project "App"
    staticruntime "off"
    common_language_spec()
    common_target()
    common_directories()

    externalincludedirs { application_externalincludedirs() }
    links { application_links() }

    filter "configurations:Debug"
        kind "ConsoleApp"
        application_debug_settings()

    filter "configurations:Release"
        kind "ConsoleApp"
        application_release_settings()

    filter "configurations:Dist"
        kind "WindowedApp"
        application_dist_settings()

    filter "system:windows"
        windows_settings()

    filter "system:linux"
        linux_settings()

    filter "action:xcode4"
        xcode_settings()
        xcodebuildsettings {
            ["INFOPLIST_FILE"] = "Info.plist",
            ["PRODUCT_BUNDLE_IDENTIFIER"] = "com.mixture.app"
        }

    filter {}
