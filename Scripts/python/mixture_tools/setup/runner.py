import logging
import os
import platform

from ..context import Context
from ..errors import SetupError
from ..prompting import PromptPolicy
from .git import update_submodules
from .premake import ensure_premake, run_premake
from .python import PythonConfiguration
from .visual_studio import get_premake_target
from .vulkan import VulkanConfiguration


logger = logging.getLogger(__name__)


def run_setup(context: Context, prompts: PromptPolicy) -> int:
    logger.info("Working in: %s", context.repository_root)

    requirements = context.configuration.requirements
    if not PythonConfiguration.validate(requirements.python):
        raise SetupError("Python setup failed.")

    system = platform.system()
    premake_binary = ensure_premake(
        context.repository_root,
        context.configuration.premake,
        prompts,
        system=system,
    )

    if not VulkanConfiguration.validate(requirements.vulkan):
        raise SetupError("Vulkan setup failed.")

    update_submodules(context.repository_root)

    if premake_binary is None:
        raise SetupError("Project requires Premake to generate project files.")

    match system:
        case "Windows":
            arguments = [get_premake_target(context.repository_root)]
        case "Linux":
            arguments = ["--cc=gcc", "gmake2"]
        case "Darwin":
            arguments = ["--cc=clang", "xcode4"]
        case _:
            raise SetupError(f"Unsupported system: {system}")

    run_premake(premake_binary, arguments, context.repository_root)

    if os.environ.get("TERM_PROGRAM") == "vscode":
        run_premake(premake_binary, ["vscode"], context.repository_root)

    return 0
