import logging
import os
import re
import platform
from pathlib import Path
from typing import Tuple, Optional

logger = logging.getLogger(__name__)

class VulkanConfiguration:
    version_pattern = r"v?(?P<major>\d+)\.(?P<minor>\d+)\.(?P<patch>\d+)"
    required_vulkan_version: Tuple[int, int, int] = (1, 3, 216)

    @classmethod
    def validate(cls) -> bool:
        if not cls.check_vulkan_sdk():
            logger.error("Vulkan SDK not installed correctly.")
            return False

        if not cls.check_vulkan_sdk_debug_libs():
            logger.warning("No Vulkan SDK debug libs found. Install Vulkan SDK with debug libs.")
            # Not returning False here, as we might still want to proceed without debug libs

        return True

    @classmethod
    def check_vulkan_sdk(cls) -> bool:
        vulkan_sdk_env = os.environ.get("VULKAN_SDK")
        if vulkan_sdk_env is None:
            logger.error("You don't have the Vulkan SDK installed!")
            return False

        vulkan_path = Path(vulkan_sdk_env)
        logger.info(f"Valid Vulkan SDK located at {vulkan_path}")

        # Try to parse version from path name (common convention) or other means if necessary
        # Usually VULKAN_SDK points to something like C:\VulkanSDK\1.3.216.0
        # The parent folder name or the folder name itself often contains the version.
        # But here we stick to the regex matching on the env var string as per original logic.
        match = re.search(cls.version_pattern, vulkan_sdk_env)
        if match:
            found_version = (
                int(match.group("major")),
                int(match.group("minor")),
                int(match.group("patch"))
            )

            if found_version < cls.required_vulkan_version:
                logger.error(f"You don't have a valid Vulkan SDK version! (Minimum {cls.required_vulkan_version} is required)")
                return False

        return True

    @classmethod
    def check_vulkan_sdk_debug_libs(cls) -> bool:
        vulkan_sdk_env = os.environ.get("VULKAN_SDK")
        if not vulkan_sdk_env:
            return False

        vulkan_path = Path(vulkan_sdk_env)
        system = platform.system()

        shaderc_lib: Optional[Path] = None

        if system == "Windows":
            shaderc_lib = vulkan_path / "Lib" / "shaderc_sharedd.lib"
        elif system == "Darwin":
            shaderc_lib = vulkan_path / "lib" / "libshaderc_shared.dylib"
        elif system == "Linux":
            shaderc_lib = vulkan_path / "lib" / "libshaderc_shared.so"

        if shaderc_lib and shaderc_lib.exists():
            logger.info(f"Vulkan SDK is installed with debug libs.")
            return True

        return False

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    VulkanConfiguration.validate()
