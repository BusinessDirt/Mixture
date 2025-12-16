import os
import re
from pathlib import Path

class VulkanConfiguration:
    versionPattern = r"v?(?P<major>\d+)\.(?P<minor>\d+)\.(?P<patch>\d+)"
    vulkanDirectory = "./vendor/VulkanSDK"
    requiredVulkanVersion = (1, 3, 216)

    @classmethod
    def validate(cls):
        if (not cls.check_vulkan_sdk()):
            print("Vulkan SDK not installed correctly.")
            return

        if (not cls.check_vulkan_sdk_debug_libs()):
            print("\nNo Vulkan SDK debug libs found. Install Vulkan SDK with debug libs.")
            print("Debug configuration disabled.")

    @classmethod
    def check_vulkan_sdk(cls):
        vulkanSDK = os.environ.get("VULKAN_SDK")
        if vulkanSDK is None:
            print("\nYou don't have the Vulkan SDK installed!")
            return False
        else:
            print(f"\nLocated Vulkan SDK at {vulkanSDK}")

        match = re.search(cls.versionPattern, vulkanSDK)
        if match:
            foundVersion = (
                int(match.group("major")),
                int(match.group("minor")),
                int(match.group("patch"))
            )

            if foundVersion < cls.requiredVulkanVersion:
                print(f"You don't have the correct Vulkan SDK version! (Minimum {cls.requiredVulkanVersion} is required)")
                return False

        print(f"Correct Vulkan SDK located at {vulkanSDK}")
        return True

    @classmethod
    def check_vulkan_sdk_debug_libs(cls):
        vulkanSDK = os.environ.get("VULKAN_SDK")
        shadercdLib = Path(f"{vulkanSDK}/Lib/shaderc_sharedd.lib")

        return shadercdLib.exists()

if __name__ == "__main__":
    VulkanConfiguration.Validate()
