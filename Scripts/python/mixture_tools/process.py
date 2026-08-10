import logging
import subprocess
from collections.abc import Iterable, Mapping
from dataclasses import dataclass
from pathlib import Path

from .errors import ProcessError


logger = logging.getLogger(__name__)


@dataclass(frozen=True)
class ProcessResult:
    command: tuple[str, ...]
    return_code: int
    stdout: str
    stderr: str


def run(
    command: Iterable[str | Path],
    *,
    cwd: Path,
    environment: Mapping[str, str] | None = None,
    check: bool = True,
) -> ProcessResult:
    normalized = tuple(str(argument) for argument in command)
    logger.info("Running: %s", " ".join(normalized))

    completed = subprocess.run(
        normalized,
        cwd=cwd,
        env=environment,
        capture_output=True,
        text=True,
        check=False,
    )
    result = ProcessResult(
        command=normalized,
        return_code=completed.returncode,
        stdout=completed.stdout,
        stderr=completed.stderr,
    )

    if check and result.return_code != 0:
        raise ProcessError(list(normalized), result.return_code)
    return result


def run_streaming(
    command: Iterable[str | Path],
    *,
    cwd: Path,
    check: bool = True,
) -> int:
    normalized = tuple(str(argument) for argument in command)
    logger.info("Running: %s", " ".join(normalized))

    with subprocess.Popen(
        normalized,
        cwd=cwd,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        bufsize=1,
    ) as process:
        assert process.stdout is not None
        for line in process.stdout:
            message = line.rstrip()
            if message.startswith("Error: "):
                logger.error("%s", message.removeprefix("Error: "))
            else:
                logger.info("%s", message)

    if check and process.returncode != 0:
        raise ProcessError(list(normalized), process.returncode)
    return process.returncode
