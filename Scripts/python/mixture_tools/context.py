import re
import tomllib
from dataclasses import dataclass
from pathlib import Path

from .errors import ConfigurationError


SHA256_PATTERN = re.compile(r"^[0-9a-f]{64}$")


@dataclass(frozen=True)
class DownloadConfiguration:
    url: str
    sha256: str


@dataclass(frozen=True)
class PremakePlatformConfiguration:
    archive: str
    executable: str
    sha256: str


@dataclass(frozen=True)
class PremakeConfiguration:
    version: str
    base_url: str
    license: DownloadConfiguration
    platforms: dict[str, PremakePlatformConfiguration]


@dataclass(frozen=True)
class RequirementsConfiguration:
    python: tuple[int, int]
    vulkan: tuple[int, int, int]


@dataclass(frozen=True)
class ProjectConfiguration:
    premake: PremakeConfiguration
    requirements: RequirementsConfiguration


@dataclass(frozen=True)
class Context:
    repository_root: Path
    configuration_file: Path
    configuration: ProjectConfiguration

    @classmethod
    def discover(cls) -> "Context":
        repository_root = Path(__file__).resolve().parents[3]

        configuration_file = repository_root / "mixture.toml"
        return cls(
            repository_root=repository_root,
            configuration_file=configuration_file,
            configuration=load_configuration(configuration_file),
        )


def _require_string(mapping: dict, key: str, location: str) -> str:
    value = mapping.get(key)
    if not isinstance(value, str) or not value:
        raise ConfigurationError(f"{location}.{key} must be a non-empty string")
    return value


def _require_sha256(mapping: dict, key: str, location: str) -> str:
    value = _require_string(mapping, key, location).lower()
    if SHA256_PATTERN.fullmatch(value) is None:
        raise ConfigurationError(f"{location}.{key} must be 64 hexadecimal characters")
    return value


def _parse_numeric_version(value: str, parts: int, location: str) -> tuple[int, ...]:
    components = value.split(".")
    if len(components) != parts or any(not component.isdigit() for component in components):
        expected = ".".join("N" for _ in range(parts))
        raise ConfigurationError(f"{location} must use {expected} format")
    return tuple(int(component) for component in components)


def load_configuration(configuration_file: Path) -> ProjectConfiguration:
    try:
        with configuration_file.open("rb") as file:
            document = tomllib.load(file)
        premake = document["tools"]["premake"]
        license_configuration = premake["license"]
        platform_configurations = premake["platforms"]
        requirements = document["requirements"]
    except FileNotFoundError as error:
        raise ConfigurationError(f"Configuration file not found: {configuration_file}") from error
    except tomllib.TOMLDecodeError as error:
        raise ConfigurationError(f"Invalid TOML in {configuration_file}: {error}") from error
    except (KeyError, TypeError) as error:
        raise ConfigurationError(f"Missing configuration entry: {error}") from error

    if not isinstance(platform_configurations, dict) or not platform_configurations:
        raise ConfigurationError("tools.premake.platforms must contain at least one platform")

    platforms: dict[str, PremakePlatformConfiguration] = {}
    for name, values in platform_configurations.items():
        location = f"tools.premake.platforms.{name}"
        if not isinstance(values, dict):
            raise ConfigurationError(f"{location} must be a table")
        platforms[name] = PremakePlatformConfiguration(
            archive=_require_string(values, "archive", location),
            executable=_require_string(values, "executable", location),
            sha256=_require_sha256(values, "sha256", location),
        )

    python_version = _parse_numeric_version(
        _require_string(requirements, "python", "requirements"), 2, "requirements.python"
    )
    vulkan_version = _parse_numeric_version(
        _require_string(requirements, "vulkan", "requirements"), 3, "requirements.vulkan"
    )

    return ProjectConfiguration(
        premake=PremakeConfiguration(
            version=_require_string(premake, "version", "tools.premake"),
            base_url=_require_string(premake, "base_url", "tools.premake").rstrip("/"),
            license=DownloadConfiguration(
                url=_require_string(license_configuration, "url", "tools.premake.license"),
                sha256=_require_sha256(license_configuration, "sha256", "tools.premake.license"),
            ),
            platforms=platforms,
        ),
        requirements=RequirementsConfiguration(
            python=(python_version[0], python_version[1]),
            vulkan=(vulkan_version[0], vulkan_version[1], vulkan_version[2]),
        ),
    )
