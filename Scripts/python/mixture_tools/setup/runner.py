import logging
import os
import platform
import sys
from pathlib import Path


class LogFormatter(logging.Formatter):
    # ANSI Escape Codes
    grey = "\x1b[38;20m"
    blue = "\x1b[34;20m"
    green = "\x1b[32;20m"
    yellow = "\x1b[33;20m"
    red = "\x1b[31;20m"
    bold_red = "\x1b[31;1m"
    bold_pink = "\x1b[1;35m" # For logger name (markers)
    reset = "\x1b[0m"

    LEVEL_COLORS = {
        logging.DEBUG: blue,
        logging.INFO: green,
        logging.WARNING: yellow,
        logging.ERROR: red,
        logging.CRITICAL: bold_red
    }

    def format(self, record):
        # Ensure time is formatted
        record.asctime = self.formatTime(record, self.datefmt)

        # Determine colors
        level_color = self.LEVEL_COLORS.get(record.levelno, self.grey)

        # Format Logger Name (Base.Marker)
        # "marker only things after the . should be marked"
        name = record.name
        if "." in name:
            base, marker = name.split(".", 1)
            # Base (Default) + .Marker (Pink)
            fmt_name = f"{base}{self.bold_pink}/{marker}{self.reset}"
        else:
            fmt_name = name

        # Format Message
        message = record.getMessage()

        log_str = (
            f"[{record.asctime}] "
            f"[{record.threadName}/{level_color}{record.levelname}{self.reset}] "
            f"({fmt_name}): "
            f"{level_color}{message}{self.reset}"
        )

        return log_str


def setup_logging() -> logging.Logger:
    handler = logging.StreamHandler()
    handler.setFormatter(LogFormatter(datefmt='%H:%M:%S'))
    logging.basicConfig(level=logging.INFO, handlers=[handler])
    return logging.getLogger("Setup")


# Configure logging
logger = setup_logging()

# Ensure the scripts directory is in sys.path to allow imports from local modules
script_dir = Path(__file__).resolve().parent
python_dir = script_dir.parents[1]
if str(python_dir) not in sys.path:
    sys.path.append(str(python_dir))

# Change working directory to the project root
project_root = script_dir.parents[3]
os.chdir(project_root)
logger.info(f"Working in: {os.getcwd()}")

# python setup
from .python import PythonConfiguration as PythonRequirements
if not PythonRequirements.validate():
    logger.error("Python setup failed.")
    sys.exit(1)

# premake setup
from .premake import PremakeConfiguration as PremakeRequirements
from .premake import run_premake
premake_installed = PremakeRequirements.validate()

# other requirements
from .vulkan import VulkanConfiguration as VulkanRequirements
if not VulkanRequirements.validate():
    logger.error("Vulkan setup failed.")
    sys.exit(1)

from .visual_studio import get_premake_target
from .git import update_submodules

# submodules and build files
update_submodules(project_root)

if premake_installed:
    system = platform.system()
    scripts_path = project_root / "Scripts"
    premake_binary_path = project_root / "vendor" / "premake" / "bin"

    logger.info(f"Running premake ({system})...")

    match system:
        case "Windows":
            binary, args = premake_binary_path / "premake5.exe", [get_premake_target()]
        case "Linux":
            binary, args = premake_binary_path / "premake5", ["--cc=gcc", "gmake2"]
        case "Darwin":
            binary, args = premake_binary_path / "premake5", ["--cc=clang", "xcode4"]
        case _:
            logger.error(f"Unsupported system: {system}")
            sys.exit(-1)

    run_premake(binary, args)

    if os.environ.get("TERM_PROGRAM") == "vscode":
        run_premake(binary, ["vscode"])

else:
    logger.error("Project requires Premake to generate project files.")
