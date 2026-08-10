import logging
from pathlib import Path

from ..process import run


logger = logging.getLogger(__name__)


def update_submodules(repository_root: Path) -> None:
    for command in (
        ["git", "submodule", "sync"],
        ["git", "submodule", "update", "--init", "--recursive"],
    ):
        result = run(command, cwd=repository_root)
        if result.stdout.strip():
            for line in result.stdout.splitlines():
                logger.info(" - %s", line)
        else:
            logger.info(" - Done. No changes needed")
