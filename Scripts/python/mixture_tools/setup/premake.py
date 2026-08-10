import logging
import platform
from pathlib import Path

from ..context import PremakeConfiguration, PremakePlatformConfiguration
from ..downloads.archive import unzip_file
from ..downloads.download import download_file
from ..errors import ConfigurationError, SetupError
from ..process import run_streaming
from ..prompting import PromptPolicy


logger = logging.getLogger(__name__)

PLATFORM_NAMES = {
    "Windows": "windows",
    "Linux": "linux",
    "Darwin": "macos",
}


def platform_release(
    configuration: PremakeConfiguration,
    system: str | None = None,
) -> PremakePlatformConfiguration:
    host_system = system or platform.system()
    try:
        platform_name = PLATFORM_NAMES[host_system]
        return configuration.platforms[platform_name]
    except KeyError as error:
        raise ConfigurationError(f"Unsupported platform: {host_system}") from error


def ensure_premake(
    repository_root: Path,
    configuration: PremakeConfiguration,
    prompts: PromptPolicy,
    *,
    system: str | None = None,
) -> Path | None:
    release = platform_release(configuration, system)
    installation_directory = repository_root / "vendor" / "premake" / "bin"
    executable = installation_directory / release.executable

    if executable.exists():
        logger.info("Correct Premake located at %s", installation_directory)
        return executable

    if not prompts.confirm(
        f"Premake {configuration.version} is not installed. Download it?"
    ):
        return None

    archive_path = installation_directory / release.archive
    archive_url = f"{configuration.base_url}/{release.archive}"

    try:
        download_file(archive_url, archive_path, release.sha256)
        unzip_file(archive_path, delete_zip_file=True)

        if (system or platform.system()) != "Windows":
            executable.chmod(executable.stat().st_mode | 0o111)

        license_path = installation_directory / "LICENSE.txt"
        download_file(
            configuration.license.url,
            license_path,
            configuration.license.sha256,
        )
    except (OSError, RuntimeError, ValueError) as error:
        raise SetupError(f"Failed to install Premake {configuration.version}: {error}") from error

    logger.info(
        "Premake %s has been downloaded to '%s'",
        configuration.version,
        installation_directory,
    )
    return executable


def run_premake(binary: Path, arguments: list[str], repository_root: Path) -> None:
    run_streaming([binary, *arguments], cwd=repository_root)
