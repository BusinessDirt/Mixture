import tempfile
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from mixture_tools.version import Version, read_version, write_version


class VersionTests(unittest.TestCase):
    def test_parses_and_bumps_semantic_version(self):
        version = Version.parse("1.2.3")

        self.assertEqual(str(version.bump("major")), "2.0.0")
        self.assertEqual(str(version.bump("minor")), "1.3.0")
        self.assertEqual(str(version.bump("patch")), "1.2.4")

    def test_rejects_invalid_version(self):
        for value in ("1.2", "v1.2.3", "01.2.3", "1.2.3-beta"):
            with self.subTest(value=value), self.assertRaises(ValueError):
                Version.parse(value)

    def test_updates_only_engine_version(self):
        with tempfile.TemporaryDirectory() as directory:
            configuration = Path(directory) / "mixture.toml"
            configuration.write_text(
                '[engine]\nversion = "1.2.3"\n\n[tools.premake]\nversion = "5.0.0"\n',
                encoding="utf-8",
            )

            write_version(configuration, Version(2, 0, 0))

            self.assertEqual(read_version(configuration), Version(2, 0, 0))
            self.assertIn('version = "5.0.0"', configuration.read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
