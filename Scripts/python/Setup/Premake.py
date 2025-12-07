import sys
import os
import platform
from pathlib import Path

import Utils

class PremakeConfiguration:
    premakeVersion = "5.0.0-beta7"
    premakeZipUrl = f"https://github.com/premake/premake-core/releases/download/v{premakeVersion}/premake-{premakeVersion}-"
    premakeLicenseUrl = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"
    premakeDirectory = "./vendor/premake/bin"

    @classmethod
    def validate(cls):
        if not cls.check_if_premake_is_installed():
            print("Premake is not installed.")
            return False

        print(f"Correct Premake located at {os.path.abspath(cls.premakeDirectory)}")
        return True

    @classmethod
    def check_if_premake_is_installed(cls):
        if platform.system() == "Windows":
            return cls.__check_if_premake_is_installed_helper("premake5.exe", "windows.zip")
        
        if platform.system() == "Linux":
            return cls.__check_if_premake_is_installed_helper("premake5", "linux.tar.gz")
        
        if platform.system() == "Darwin":
            return cls.__check_if_premake_is_installed_helper("premake5", "macosx.tar.gz")
        
    @classmethod
    def __check_if_premake_is_installed_helper(cls, binary: str, distribution: str) -> bool:
        premakeExe = Path(f"{cls.premakeDirectory}/{binary}");
        if (not premakeExe.exists()):
            return cls.InstallPremake(distribution)
        return True

    @classmethod
    def InstallPremake(cls, distribution):
        permissionGranted = False
        while not permissionGranted:
            if os.getenv("CI"):
                permissionGranted = True
                break
            reply = input("Premake not found. Would you like to download Premake {0:s}? [Y/N]: ".format(cls.premakeVersion)).lower().strip()
            if reply == 'n':
                return False
            permissionGranted = (reply == 'y')

        premakePath = f"{cls.premakeDirectory}/premake-{cls.premakeVersion}-{distribution}"
        print("Downloading {0:s} to {1:s}".format(cls.premakeZipUrl, premakePath))
        Utils.download_file(cls.premakeZipUrl + distribution, premakePath)
        print("Extracting", premakePath)
        Utils.unzip_file(premakePath, delete_zip_file=True)
        print(f"Premake {cls.premakeVersion} has been downloaded to '{cls.premakeDirectory}'")

        premakeLicensePath = f"{cls.premakeDirectory}/LICENSE.txt"
        print("Downloading {0:s} to {1:s}".format(cls.premakeLicenseUrl, premakeLicensePath))
        Utils.download_file(cls.premakeLicenseUrl, premakeLicensePath)
        print(f"Premake License file has been downloaded to '{cls.premakeDirectory}'")

        return True