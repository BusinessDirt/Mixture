import os
import re
import tempfile
import tomllib
from dataclasses import dataclass
from pathlib import Path
from typing import Literal


VERSION_PATTERN = re.compile(
    r"^(?P<major>0|[1-9]\d*)\."
    r"(?P<minor>0|[1-9]\d*)\."
    r"(?P<patch>0|[1-9]\d*)$"
)

VersionPart = Literal["major", "minor", "patch"]


@dataclass(frozen=True, order=True)
class Version:
    major: int
    minor: int
    patch: int

    @classmethod
    def parse(cls, value: str) -> "Version":
        match = VERSION_PATTERN.fullmatch(value.strip())

        if match is None:
            raise ValueError(
                f"Invalid version {value!r}; expected MAJOR.MINOR.PATCH"
            )

        return cls(
            major=int(match["major"]),
            minor=int(match["minor"]),
            patch=int(match["patch"]),
        )

    def bump(self, part: VersionPart) -> "Version":
        match part:
            case "major":
                return Version(self.major + 1, 0, 0)
            case "minor":
                return Version(self.major, self.minor + 1, 0)
            case "patch":
                return Version(self.major, self.minor, self.patch + 1)

    def __str__(self) -> str:
        return f"{self.major}.{self.minor}.{self.patch}"


def read_version(configuration_file: Path) -> Version:
    with configuration_file.open("rb") as file:
        configuration = tomllib.load(file)

    try:
        value = configuration["engine"]["version"]
    except (KeyError, TypeError) as error:
        raise ValueError(
            f"{configuration_file} does not contain engine.version"
        ) from error

    if not isinstance(value, str):
        raise ValueError("engine.version must be a string")

    return Version.parse(value)


def write_version(configuration_file: Path, version: Version) -> None:
    contents = configuration_file.read_text(encoding="utf-8")

    pattern = re.compile(
        r'(?m)^(version\s*=\s*")[^"]*(")\s*$'
    )

    updated, replacements = pattern.subn(
        rf"\g<1>{version}\g<2>",
        contents,
        count=1,
    )

    if replacements != 1:
        raise ValueError(
            f"Could not uniquely locate the version in {configuration_file}"
        )

    temporary_path: Path | None = None

    try:
        with tempfile.NamedTemporaryFile(
            mode="w",
            encoding="utf-8",
            dir=configuration_file.parent,
            prefix=f".{configuration_file.name}.",
            delete=False,
        ) as temporary_file:
            temporary_file.write(updated)
            temporary_path = Path(temporary_file.name)

        os.replace(temporary_path, configuration_file)
        temporary_path = None
    finally:
        if temporary_path is not None:
            temporary_path.unlink(missing_ok=True)
