import os
import subprocess
import platform
import sys
from pathlib import Path

# Configure logging
import Logging
logger = Logging.setup()

# Ensure the script directory is in sys.path to allow imports from local modules
script_dir = Path(__file__).resolve().parent
if str(script_dir) not in sys.path:
    sys.path.append(str(script_dir))

# Change working directory to the project root
project_root = script_dir.parents[1]
os.chdir(project_root)
logger.info(f"Working in: {os.getcwd()}")

# python setup
from Setup.Python import PythonConfiguration as PythonRequirements
if not PythonRequirements.validate():
    logger.error("Python setup failed.")
    sys.exit(1)

# premake setup
from Setup.Premake import PremakeConfiguration as PremakeRequirements
premake_installed = PremakeRequirements.validate()

# other requirements
from Setup.Vulkan import VulkanConfiguration as VulkanRequirements
if not VulkanRequirements.validate():
    logger.error("Vulkan setup failed.")
    sys.exit(1)

from Setup.VisualStudio import get_premake_target
from Setup.Git import update_submodules

# submodules and build files
update_submodules(project_root)

if premake_installed:
    system = platform.system()
    scripts_path = project_root / "Scripts"
    premake_binary_path = project_root / "vendor" / "premake" / "bin"

    try:
        if system == "Windows":
            logger.info("Running premake (Windows)...")
            subprocess.run([premake_binary_path / "premake5.exe", get_premake_target()], check=True)

        elif system == "Linux":
            logger.info("Running premake (Linux)...")
            subprocess.run([premake_binary_path / "premake5", "--cc=clang", "gmake2"], check=True)

        elif system == "Darwin":
            logger.info("Running premake (MacOS)...")
            subprocess.run([premake_binary_path / "premake5", "--cc=clang", "xcode4"], check=True)

        logger.info("Setup completed!")

    except subprocess.CalledProcessError as e:
        logger.error(f"Error during project generation: {e}")
        sys.exit(1)
else:
    logger.error("Project requires Premake to generate project files.")


