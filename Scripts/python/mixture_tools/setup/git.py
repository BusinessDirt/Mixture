import logging
import subprocess
import sys

logger = logging.getLogger(__name__)

def update_submodules(repo_root: str):
    commands = [
        ["git", "submodule", "sync"],
        ["git", "submodule", "update", "--init", "--recursive"]
    ]

    for cmd in commands:
        logger.info(f"Running: {' '.join(cmd)}...")
        try:
            # Run inside the repo root!
            result = subprocess.run(
                cmd,
                cwd=repo_root,
                check=True,
                capture_output=True,
                text=True
            )
            # If successful, print stdout (might be empty if up to date)
            if result.stdout.strip():
                for line in result.stdout.splitlines():
                    logger.info(" - " + line)
            else:
                logger.info(" - Done. No changes needed")

        except subprocess.CalledProcessError:
            logger.exception(f"Error running command: {' '.join(cmd)}")
            sys.exit(1)

if __name__ == "__main__":
    update_submodules()
