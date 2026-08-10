class MixtureToolsError(Exception):
    """Base exception for expected tool failures."""


class ConfigurationError(MixtureToolsError):
    """Raised when mixture.toml is missing or invalid."""


class SetupError(MixtureToolsError):
    """Raised when a setup requirement cannot be satisfied."""


class ProcessError(MixtureToolsError):
    """Raised when an external command fails."""

    def __init__(
        self,
        command: list[str],
        return_code: int,
    ) -> None:
        self.command = command
        self.return_code = return_code

        super().__init__(
            f"Command failed with exit code {return_code}: "
            f"{' '.join(command)}"
        )


class PermissionDeniedError(MixtureToolsError):
    """Raised when an operation is rejected or cannot request approval."""
