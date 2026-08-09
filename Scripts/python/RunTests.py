import argparse
import subprocess
import sys
from pathlib import Path


def find_test_executable(build_root: Path, configuration: str) -> Path:
    executable_names = {"Tests", "Tests.exe"}
    candidates = sorted(
        path.resolve()
        for path in build_root.glob(f"{configuration}-*/Tests/*")
        if path.is_file() and path.name in executable_names
    )

    if not candidates:
        raise FileNotFoundError(
            f"No {configuration} test executable found below {build_root}"
        )
    if len(candidates) > 1:
        formatted = "\n".join(f"  - {candidate}" for candidate in candidates)
        raise RuntimeError(f"Multiple test executables found:\n{formatted}")
    return candidates[0]


def main() -> int:
    parser = argparse.ArgumentParser(description="Run the generated Mixture test binary")
    parser.add_argument("--configuration", default="Release")
    parser.add_argument("--build-root", type=Path, default=Path("bin"))
    parser.add_argument("--output", type=Path, default=Path("test-results/results.xml"))
    args = parser.parse_args()

    repository_root = Path(__file__).resolve().parents[2]
    build_root = args.build_root
    if not build_root.is_absolute():
        build_root = repository_root / build_root

    output = args.output
    if not output.is_absolute():
        output = repository_root / output
    output.parent.mkdir(parents=True, exist_ok=True)

    try:
        executable = find_test_executable(build_root, args.configuration)
    except (FileNotFoundError, RuntimeError) as error:
        print(error, file=sys.stderr)
        return 1

    result = subprocess.run(
        [str(executable), f"--gtest_output=xml:{output}"],
        cwd=repository_root,
        check=False,
    )
    return result.returncode


if __name__ == "__main__":
    sys.exit(main())
