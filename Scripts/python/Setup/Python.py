import sys
import os
import platform
import subprocess
import importlib.util as importlib_util

class PythonConfiguration:
    @classmethod
    def validate(cls) -> bool:
        if not cls.__validate_python__():
            return False # cannot validate further
        
        for package_name in ["requests"]:
            if not cls.__validate_package__(package_name):
                return False # cannot validate further

        return True

    @classmethod
    def __validate_python__(cls, version_major: int = 3, version_minor: int = 3) -> bool:
        if sys.version is not None:
            print(f"Python version {sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro} detected")
            if sys.version_info.major < version_major or (sys.version_info.major == version_major and sys.version_info.minor < version_minor):
                print(f"Python version too low, expected version {version_major}.{version_minor} or higher.")
                return False
            return True
        
    @classmethod
    def __validate_package__(cls, package_name: str) -> bool:
        if importlib_util.find_spec(package_name) is None:
            return cls.__install_package__(package_name)
        return True
    
    @classmethod
    def __install_package__(cls, package_name: str) -> bool:
        permission_granted: bool = False
        while not permission_granted:
            if os.getenv("CI"):
                permission_granted = True
                break
            user_input: str = input(f"Would you like to install Python package '{package_name}'? [Y/N]: ").lower().strip()
            if (user_input == 'n'):
                return False
            permission_granted = (user_input == 'y')

        print(f"Installing {package_name} module...")
        subprocess.check_call([cls.os_specific_python_command(), "-m", "pip", "install", package_name])

        return cls.__validate_package__(package_name)

    @classmethod
    def os_specific_python_command(cls) -> str:
        if platform.system() == "Windows": return "py"
        if platform.system() == "Linux": return "python"
        if platform.system() == "Darwin": return "python3"
    
if __name__ == "__main__":
    validated: bool = PythonConfiguration.validate()
    print(f"Validation successful: {validated}")