-- Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/include"
IncludeDir["glm"] = "%{wks.location}/vendor/glm"
IncludeDir["glfw"] = "%{wks.location}/vendor/glfw/include"
IncludeDir["spdlog"] = "%{wks.location}/vendor/spdlog/include"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/lib"

Library = {}

filter "system:windows"
    Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"

filter "action:xcode4"
    Library["Vulkan"] = "%{VULKAN_SDK}/Frameworks"