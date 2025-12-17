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

# submodules and build files
logger.info("Updating submodules...")
try:
    subprocess.run(["git", "submodule", "update", "--init", "--recursive"], check=True)
except subprocess.CalledProcessError as e:
    logger.warning(f"Failed to update submodules: {e}")
    # Proceeding as it might not be fatal if already updated, or user can fix git issues manually

if premake_installed:
    system = platform.system()
    scripts_path = project_root / "Scripts"

    try:
        if system == "Windows":
            logger.info("Running premake (Windows)...")
            script_path = scripts_path / "windows" / "Win-GenProjects.ps1"
            # PowerShell execution might need 'powershell' command prefix if not default shell,
            # but usually usually subprocess handles .ps1 if associated or via shell=True/powershell cmd.
            # The original code used os.path.abspath, so we keep that intent but call powershell explicitly for safety.
            subprocess.run(["powershell", "-ExecutionPolicy", "Bypass", "-File", str(script_path), get_premake_target(), "nopause"], check=True)

        elif system == "Linux":
            logger.info("Running premake (Linux)...")
            script_path = scripts_path / "linux" / "Linux-GenProjects.sh"
            subprocess.run(["sh", str(script_path)], check=True)

        elif system == "Darwin":
            logger.info("Running premake (MacOS)...")
            script_path = scripts_path / "macosx" / "MacOSX-GenProjects.sh"
            subprocess.run(["sh", str(script_path)], check=True)

        logger.info("Setup completed!")

    except subprocess.CalledProcessError as e:
        logger.error(f"Error during project generation: {e}")
        sys.exit(1)
else:
    logger.error("Project requires Premake to generate project files.")


