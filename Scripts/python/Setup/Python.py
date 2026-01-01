import logging
import sys
import os
import subprocess
import importlib.util as importlib_util

logger = logging.getLogger(__name__)

class PythonConfiguration:
    @classmethod
    def validate(cls) -> bool:
        if not cls._validate_python():
            return False

        # Check for required packages
        required_packages = ["requests"]
        for package_name in required_packages:
            if not cls._validate_package(package_name):
                return False

        return True

    @classmethod
    def _validate_python(cls, version_major: int = 3, version_minor: int = 3) -> bool:
        if sys.version_info < (version_major, version_minor):
            logger.error(f"Python version too low, expected version {version_major}.{version_minor} or higher.")
            return False

        logger.info(f"Python {sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro} detected")
        return True

    @classmethod
    def _validate_package(cls, package_name: str) -> bool:
        if importlib_util.find_spec(package_name) is None:
            return cls._install_package(package_name)
        return True

    @classmethod
    def _install_package(cls, package_name: str) -> bool:
        logger.info(f"Installing {package_name} module...")

        permission_granted = False
        if os.getenv("CI"):
            logger.info(f"CI environment detected. Auto-approving download of Premake {cls.premake_version}.")
            permission_granted = True
        else:
            permission_granted = False

        while not permission_granted:
            user_input = input(f"Would you like to install Python package '{package_name}'? [Y/N]: ").lower().strip()
            if user_input == 'n':
                return False
            permission_granted = (user_input == 'y')

        try:
            subprocess.check_call([sys.executable, "-m", "pip", "install", package_name])
            return cls._validate_package(package_name)
        except subprocess.CalledProcessError as e:
            logger.error(f"Failed to install package '{package_name}': {e}")
            return False

if __name__ == "__main__":
    # Basic logging config if run standalone
    logging.basicConfig(level=logging.INFO)
    validated = PythonConfiguration.validate()
    logger.info(f"Validation successful: {validated}")
