import logging
import os
import subprocess
import json
from pathlib import Path

logger = logging.getLogger(__name__)

def get_premake_target() -> str:
    """
    Detects the latest installed Visual Studio version and returns
    the corresponding Premake target string (e.g., 'vs2022').
    """

    # Locate vswhere.exe (Standard location)
    program_files = os.environ.get("ProgramFiles(x86)", "C:/Program Files (x86)")
    vswhere_path = Path(program_files) / "Microsoft Visual Studio" / "Installer" / "vswhere.exe"

    if not vswhere_path.exists():
        # Only print if we are on Windows, otherwise it's expected not to be found
        if os.name == 'nt':
            logger.warning("vswhere.exe not found. Is Visual Studio installed?")
        return "vs2022" # Fallback default

    # Run vswhere command
    cmd = [
        str(vswhere_path),
        "-all", "-prerelease",
        "-latest",
        "-products", "*",
        "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
        "-format", "json",
        "-utf8"
    ]

    try:
        result = subprocess.run(cmd, capture_output=True, text=True)

        if result.returncode != 0:
            return "vs2022"

        data = json.loads(result.stdout)

        if not data:
            logger.warning("No suitable Visual Studio installation found.")
            return "vs2022"

        full_version = data[0].get("installationVersion", "")
        major_version = int(full_version.split('.')[0])

        if major_version == 18:
            return "vs2026"
        elif major_version == 17:
            return "vs2022"
        elif major_version == 16:
            return "vs2019"
        elif major_version == 15:
            return "vs2017"
        else:
            logger.warning(f"Unknown VS version ({major_version}), defaulting to vs2022")
            return "vs2022"

    except Exception as e:
        logger.error(f"An error occurred detecting VS version: {e}")
        return "vs2022"


if __name__ == "__main__":
    target = get_premake_target()
    logger.info(target)
