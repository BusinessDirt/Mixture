import subprocess
import logging


logger = logging.getLogger(__name__)


def _run_git(args: list[str], cwd: str | None = None) -> str:
    """Helper to run a git command and return its standard output as a string."""
    result = subprocess.run(
        ["git"] + args,
        cwd=cwd,
        capture_output=True,
        text=True,
        check=False
    )
    return result.stdout.strip()

def _get_submodules() -> list[str]:
    """Returns a list of all submodule paths in the current repository."""
    output = _run_git(["submodule", "status"])
    paths = []

    for line in output.splitlines():
        if not line:
            continue
        # git submodule status lines look like: " <commit> <path> (<describe>)"
        parts = line.strip().split()
        if len(parts) >= 2:
            paths.append(parts[1])

    return paths

def update() -> list[str]:
    """
    Checks the remote repository for all submodules and lists available updates.
    Returns a list of submodule paths that have updates available.
    """
    submodules = _get_submodules()
    available_updates = []

    logger.info("Checking for submodule updates... (this might take a moment)")

    for sm in submodules:
        subprocess.run(["git", "fetch"], cwd=sm, capture_output=True)
        current_commit = _run_git(["rev-parse", "HEAD"], cwd=sm)

        tracked_branch = _run_git(["config", "-f", ".gitmodules", "--get", f"submodule.{sm}.branch"])
        if not tracked_branch:
            tracked_branch = "HEAD"

        ls_remote_output = _run_git(["ls-remote", "origin", tracked_branch], cwd=sm)

        if not ls_remote_output:
            continue

        remote_commit = ls_remote_output.split()[0]

        if current_commit != remote_commit:
            available_updates.append((sm, current_commit[:7], remote_commit[:7]))

    if not available_updates:
        logger.info("All submodules are already up to date.")
    else:
        logger.info("Updates available:")
        for sm, current, remote in available_updates:
            logger.info(f"  - {sm}: {current} -> {remote}")

    return [sm for sm, _, _ in available_updates]

def upgrade(submodules: list[str] | None = None) -> None:
    """
    Updates the specified submodules to their newest remote tracking version.
    If no list is provided, upgrades all submodules.
    """
    if not submodules:
        logger.info("Upgrading all submodules to their latest remote versions...")
        subprocess.run(["git", "submodule", "update", "--remote", "--merge"])
    else:
        logger.info(f"Upgrading {len(submodules)} specified submodule(s)...")
        args = ["git", "submodule", "update", "--remote", "--merge"] + submodules
        subprocess.run(args)

    logger.info("Upgrade complete.")
