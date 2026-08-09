import logging
import platform
import sys
import os
import subprocess
from pathlib import Path
import Utils

logger = logging.getLogger(__name__)

def run_premake(binary: str, args: list):
    cmd = [binary] + args
    with subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, bufsize=1) as process:
        for line in process.stdout:
            if ("Error: " in line):
                logger.error(line.replace("Error: ", "").strip())
            else:
                logger.info(line.strip())

    if process.returncode != 0:
        logger.error(f"Failed with return code {process.returncode}")
        sys.exit(process.returncode)

class PremakeConfiguration:
    premake_version = "5.0.0-beta8"
    premake_zip_url = f"https://github.com/premake/premake-core/releases/download/v{premake_version}/premake-{premake_version}-"
    premake_license_url = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"
    premake_directory = Path("./vendor/premake/bin").resolve()

    premake_sha256 = {
        "linux.tar.gz": "63edd3e7461eebdd45b500a3c7e8ad4e7a67d68f230010f9a97cbb71b4ec59c8",
        "macosx.tar.gz": "fa73a46f093fa6f17494a3d063421aa6cae3ea825a61c62dd59fc2f07a256d03",
        "windows.zip": "e64ce2ed8778e0098f63674cca61fe33941b5f0c8d9a4afd651152bdea3758ab",
    }

    @classmethod
    def validate(cls) -> bool:
        if not cls.check_if_premake_is_installed():
            logger.error("Premake is not installed.")
            return False

        logger.info(f"Correct Premake located at {cls.premake_directory}")
        return True

    @classmethod
    def check_if_premake_is_installed(cls) -> bool:
        system = platform.system()
        if system == "Windows":
            return cls._check_if_premake_is_installed_helper("premake5.exe", "windows.zip")
        elif system == "Linux":
            return cls._check_if_premake_is_installed_helper("premake5", "linux.tar.gz")
        elif system == "Darwin":
            return cls._check_if_premake_is_installed_helper("premake5", "macosx.tar.gz")
        else:
            logger.error(f"Unsupported platform: {system}")
            return False

    @classmethod
    def _check_if_premake_is_installed_helper(cls, binary: str, distribution: str) -> bool:
        premake_exe = cls.premake_directory / binary
        if not premake_exe.exists():
            return cls.install_premake(distribution)
        return True

    @classmethod
    def install_premake(cls, distribution: str) -> bool:
        # Check if we are in a CI environment
        # GitHub Actions, GitLab, etc. set the 'CI' env var to 'true'
        if os.getenv("CI"):
            logger.info(f"CI environment detected. Auto-approving download of Premake {cls.premake_version}.")
            permission_granted = True
        else:
            permission_granted = False

        while not permission_granted:
            # Assuming implicit CI environment check if needed, or pass it as arg
            # simplified for this context:
            reply = input(f"Premake not found. Would you like to download Premake {cls.premake_version}? [Y/N]: ").lower().strip()
            if reply == 'n':
                return False
            permission_granted = (reply == 'y')

        premake_path = cls.premake_directory / f"premake-{cls.premake_version}-{distribution}"

        try:
            Utils.download_file(f"{cls.premake_zip_url}{distribution}", premake_path, cls.premake_sha256[distribution])
            Utils.unzip_file(premake_path, delete_zip_file=True)

            if platform.system() != "Windows":
                premake_exe = cls.premake_directory / "premake5"
                premake_exe.chmod(premake_exe.stat().st_mode | 0o111)

            logger.info(f"Premake {cls.premake_version} has been downloaded to '{cls.premake_directory}'")

            premake_license_path = cls.premake_directory / "LICENSE.txt"
            Utils.download_file(cls.premake_license_url, premake_license_path)
            logger.info(f"Premake License has been downloaded to '{cls.premake_directory}'")

            return True
        except Exception as e:
            logger.error(f"Failed to install Premake: {e}")
            return False

