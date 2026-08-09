import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from RunTests import find_test_executable


class TestExecutableDiscoveryTests(unittest.TestCase):
    def setUp(self):
        self.temporary_directory = tempfile.TemporaryDirectory()
        self.build_root = Path(self.temporary_directory.name)

    def tearDown(self):
        self.temporary_directory.cleanup()

    def test_finds_host_architecture_output(self):
        executable = self.build_root / "Release-macosx-ARM64/Tests/Tests"
        executable.parent.mkdir(parents=True)
        executable.touch()

        self.assertEqual(
            find_test_executable(self.build_root, "Release"),
            executable.resolve(),
        )

    def test_supports_windows_executable(self):
        executable = self.build_root / "Release-windows-x86_64/Tests/Tests.exe"
        executable.parent.mkdir(parents=True)
        executable.touch()

        self.assertEqual(
            find_test_executable(self.build_root, "Release"),
            executable.resolve(),
        )

    def test_rejects_missing_executable(self):
        with self.assertRaises(FileNotFoundError):
            find_test_executable(self.build_root, "Release")

    def test_rejects_ambiguous_outputs(self):
        for architecture in ("ARM64", "x86_64"):
            executable = self.build_root / f"Release-macosx-{architecture}/Tests/Tests"
            executable.parent.mkdir(parents=True)
            executable.touch()

        with self.assertRaises(RuntimeError):
            find_test_executable(self.build_root, "Release")


if __name__ == "__main__":
    unittest.main()
