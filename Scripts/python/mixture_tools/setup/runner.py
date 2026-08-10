import logging
import os
import platform
from pathlib import Path

from .git import update_submodules
from .premake import PremakeConfiguration, run_premake
from .python import PythonConfiguration
from .visual_studio import get_premake_target
from .vulkan import VulkanConfiguration


# Configure logging
logger = logging.getLogger(__name__)

def run_setup(repository_root: Path) -> int:
    os.chdir(repository_root)
    logger.info("Working in: %s", repository_root)

    if not PythonConfiguration.validate():
        logger.error("Python setup failed.")
        return 1

    premake_installed = PremakeConfiguration.validate()

    if not VulkanConfiguration.validate():
        logger.error("Vulkan setup failed.")
        return 1

    update_submodules(repository_root)

    if not premake_installed:
        logger.error("Project requires Premake to generate project files.")
        return 1

    system = platform.system()
    binary_directory = repository_root / "vendor" / "premake" / "bin"

    match system:
        case "Windows":
            binary = binary_directory / "premake5.exe"
            arguments = [get_premake_target()]
        case "Linux":
            binary = binary_directory / "premake5"
            arguments = ["--cc=gcc", "gmake2"]
        case "Darwin":
            binary = binary_directory / "premake5"
            arguments = ["--cc=clang", "xcode4"]
        case _:
            logger.error("Unsupported system: %s", system)
            return 1

    run_premake(binary, arguments)

    if os.environ.get("TERM_PROGRAM") == "vscode":
        run_premake(binary, ["vscode"])

    return 0
