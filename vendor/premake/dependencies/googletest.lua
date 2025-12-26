-- vendor/premake/dependencies/googletest.lua

IncludeDir["googletest"] = "%{wks.location}/vendor/googletest/googletest/include"
IncludeDir["googlemock"] = "%{wks.location}/vendor/googletest/googlemock/include"

project "GoogleTest"
      kind "StaticLib"
      language "C++"
      cppdialect "C++17"

      -- Point to the googletest src/include folders
      targetdir ("bin/" .. outputdir .. "/%{prj.name}")
      objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

      includedirs {
         "%{wks.location}/vendor/googletest/googletest/include",
         "%{wks.location}/vendor/googletest/googlemock/include",
         "%{wks.location}/vendor/googletest/googletest",
         "%{wks.location}/vendor/googletest/googlemock/"
      }

      files {
         "%{wks.location}/vendor/googletest/googletest/src/gtest-all.cc",
         "%{wks.location}/vendor/googletest/googlemock/src/gmock-all.cc"
      }

      filter "configurations:Debug"
         runtime "Debug"
         symbols "On"

      filter "configurations:Release"
         runtime "Release"
         optimize "On"
