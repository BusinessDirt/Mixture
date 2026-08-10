import tempfile
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from mixture_tools.context import load_configuration
from mixture_tools.errors import ConfigurationError


class ConfigurationTests(unittest.TestCase):
    def test_loads_repository_configuration(self):
        repository_root = Path(__file__).resolve().parents[3]

        configuration = load_configuration(repository_root / "mixture.toml")

        self.assertEqual(configuration.premake.version, "5.0.0-beta8")
        self.assertEqual(configuration.requirements.python, (3, 11))
        self.assertEqual(configuration.requirements.vulkan, (1, 3, 216))
        self.assertEqual(set(configuration.premake.platforms), {"windows", "linux", "macos"})

    def test_rejects_invalid_sha256(self):
        with tempfile.TemporaryDirectory() as directory:
            source = Path(__file__).resolve().parents[3] / "mixture.toml"
            destination = Path(directory) / "mixture.toml"
            destination.write_text(
                source.read_text(encoding="utf-8").replace(
                    "e64ce2ed8778e0098f63674cca61fe33941b5f0c8d9a4afd651152bdea3758ab",
                    "invalid",
                ),
                encoding="utf-8",
            )

            with self.assertRaisesRegex(ConfigurationError, "64 hexadecimal"):
                load_configuration(destination)


if __name__ == "__main__":
    unittest.main()
