project "App"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "Binaries/%{cfg.buildcfg}"
    staticruntime "off"

    files { "src/**.hpp", "src/**.cpp" }

    includedirs {
        "src",

        -- Include Core
        "%{wks.location}/Mixture/src"
    }

    externalincludedirs {
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glm}"
    }

    links {
        "Mixture"
    }

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    -- mac specific settings
    filter "action:xcode4"
        local vulkanFramework = VULKAN_SDK .. "/Frameworks"
        local vulkanLibs = VULKAN_SDK .. "/lib"

        libdirs {
            vulkanLibs
        }

        links {
            "GLFW",
            "vulkan.framework",
            "Cocoa.framework",
            "IOKit.framework",
            "QuartzCore.framework",
            "AppKit.framework",
            "shaderc_shared",
            "spirv-cross-core",
            "spirv-cross-glsl"
        }

        externalincludedirs {
            -- need to explicitly add path to framework headers
            "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/Cocoa.framework/Headers",
            "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/IOKit.framework/Headers",
            "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/AppKit.framework/Headers",
            "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/QuartzCore.framework/Headers",
            "%{IncludeDir.VulkanSDK}"
        }

        frameworkdirs {
            -- path to search for third party frameworks
            vulkanFramework,
            "/System/Library/Frameworks",
            "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/"
        }

        xcodebuildsettings {
        --    ["MACOSX_DEPLOYMENT_TARGET"] = "14.4",
        --    ["PRODUCT_BUNDLE_IDENTIFIER"] = 'com.yourdomain.yourapp',
        --    ["CODE_SIGN_STYLE"] = "Automatic",
        --    ["DEVELOPMENT_TEAM"] = '1234ABCD56',                                    -- your dev team id
        --    ["INFOPLIST_FILE"] = "../../source/mac/Info.plist",                     -- path is relative to the generated project file
        --    ["CODE_SIGN_ENTITLEMENTS"] = ("../../source/mac/app.entitlements"),     -- ^
        --    ["ENABLE_HARDENED_RUNTIME"] = "YES",                                    -- hardened runtime is required for notarization
        --    ["CODE_SIGN_IDENTITY"] = "Apple Development",                           -- sets 'Signing Certificate' to 'Development'. Defaults to 'Sign to Run Locally'. not doing this will crash your app if you upgrade the project when prompted by Xcode
            ["LD_RUNPATH_SEARCH_PATHS"] = "@executable_path/../Frameworks @loader_path/../Frameworks " .. vulkanFramework .. " " .. vulkanLibs, -- tell the executable where to find the frameworks. Path is relative to executable location inside .app bundle
        }

    filter "configurations:Debug"
        defines { "MX_DEBUG" }
        runtime "Debug"
        symbols "On"

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