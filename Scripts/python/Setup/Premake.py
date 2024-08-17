import sys
import os
import platform
from pathlib import Path

import Utils

class PremakeConfiguration:
    BASE_URL = f"https://github.com/BusinessDirt/premake-core"
    PREMAKE_LICENCE_URL = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"
    PREMAKE_DIR = "./vendor/premake/bin"

    @classmethod
    def validate(cls):
        if not cls.check_if_premake_is_installed():
            print("Premake is not installed.")
            return False

        print(f"Correct Premake located at {os.path.abspath(cls.PREMAKE_DIR)}")
        return True

    @classmethod
    def check_if_premake_is_installed(cls):
        if platform.system() == "Windows":
            return cls.__check_if_premake_is_installed_helper("premake5.exe", "premake-windows-x64")
        
        if platform.system() == "Linux":
            return cls.__check_if_premake_is_installed_helper("premake5", "premake-linux-x64")
        
        if platform.system() == "Darwin":
            if platform.machine() == "x86_64":
                return cls.__check_if_premake_is_installed_helper("premake5", "premake-macosx-x64")
            elif platform.machine() == "arm64":
                return cls.__check_if_premake_is_installed_helper("premake5", "premake-macosx-ARM")
        
    @classmethod
    def __check_if_premake_is_installed_helper(cls, binary: str, distribution: str) -> bool:
        premakeExe = Path(f"{cls.PREMAKE_DIR}/{binary}");
        if (not premakeExe.exists()):
            return cls.InstallPremake(distribution)
        return True

    @classmethod
    def InstallPremake(cls, distribution):
        permissionGranted = False
        while not permissionGranted:
            reply = input("Premake not found. Would you like to download Premake? [Y/N]: ").lower().strip()
            if reply == 'n':
                return False
            permissionGranted = (reply == 'y')


        premakePath = f"{cls.PREMAKE_DIR}/{distribution}.zip"
        downloadURL = f"{Utils.get_latest_release(base_url=cls.BASE_URL)}/{distribution}.zip";

        print("Downloading {0:s} to {1:s}".format(downloadURL, premakePath))
        Utils.download_file(downloadURL, premakePath)
        print("Extracting", premakePath)
        Utils.unzip_file(premakePath, delete_zip_file=True)
        print(f"Premake 5 has been downloaded to '{cls.PREMAKE_DIR}'")

        premakeLicensePath = f"{cls.PREMAKE_DIR}/LICENSE.txt"
        print("Downloading {0:s} to {1:s}".format(cls.PREMAKE_LICENCE_URL, premakeLicensePath))
        Utils.download_file(cls.PREMAKE_LICENCE_URL, premakeLicensePath)
        print(f"Premake License file has been downloaded to '{cls.PREMAKE_DIR}'")

        return True