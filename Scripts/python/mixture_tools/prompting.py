import os
from dataclasses import dataclass

from .errors import PermissionDeniedError


@dataclass(frozen=True)
class PromptPolicy:
    assume_yes: bool = False
    interactive: bool = True

    @classmethod
    def from_environment(
        cls,
        *,
        assume_yes: bool = False,
        interactive: bool = True,
    ) -> "PromptPolicy":
        is_ci = os.environ.get("CI", "").lower() in {"1", "true", "yes"}
        return cls(
            assume_yes=assume_yes or is_ci,
            interactive=interactive and not is_ci,
        )

    def confirm(self, message: str) -> bool:
        if self.assume_yes:
            return True
        if not self.interactive:
            raise PermissionDeniedError(
                f"Approval required in non-interactive mode: {message}"
            )

        while True:
            response = input(f"{message} [Y/N]: ").strip().lower()
            if response in {"y", "yes"}:
                return True
            if response in {"n", "no"}:
                return False
            print("Please enter Y or N.")
