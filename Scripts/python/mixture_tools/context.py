from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class Context:
    repository_root: Path
    configuration_file: Path

    @classmethod
    def discover(cls) -> "Context":
        repository_root = Path(__file__).resolve().parents[3]

        return cls(
            repository_root=repository_root,
            configuration_file=repository_root / "mixture.toml",
        )
