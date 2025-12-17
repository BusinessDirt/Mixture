import logging
import platform
from pathlib import Path
import Utils

logger = logging.getLogger(__name__)

class PremakeConfiguration:
    premake_version = "5.0.0-beta7"
    premake_zip_url = f"https://github.com/premake/premake-core/releases/download/v{premake_version}/premake-{premake_version}-"
    premake_license_url = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"
    premake_directory = Path("./vendor/premake/bin").resolve()

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
            Utils.download_file(f"{cls.premake_zip_url}{distribution}", premake_path)
            Utils.unzip_file(premake_path, delete_zip_file=True)
            logger.info(f"Premake {cls.premake_version} has been downloaded to '{cls.premake_directory}'")

            premake_license_path = cls.premake_directory / "LICENSE.txt"
            Utils.download_file(cls.premake_license_url, premake_license_path)
            logger.info(f"Premake License has been downloaded to '{cls.premake_directory}'")

            return True
        except Exception as e:
            logger.error(f"Failed to install Premake: {e}")
            return False


