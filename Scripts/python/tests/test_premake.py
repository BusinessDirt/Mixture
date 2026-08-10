import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from mixture_tools.context import (
    DownloadConfiguration,
    PremakeConfiguration,
    PremakePlatformConfiguration,
)
from mixture_tools.prompting import PromptPolicy
from mixture_tools.setup import premake


class PremakeSetupTests(unittest.TestCase):
    def configuration(self) -> PremakeConfiguration:
        return PremakeConfiguration(
            version="1.0.0",
            base_url="https://example.test/releases",
            license=DownloadConfiguration(
                url="https://example.test/LICENSE.txt",
                sha256="b" * 64,
            ),
            platforms={
                "macos": PremakePlatformConfiguration(
                    archive="premake-macosx.tar.gz",
                    executable="premake5",
                    sha256="a" * 64,
                )
            },
        )

    def test_existing_premake_does_not_prompt_or_download(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            executable = root / "vendor/premake/bin/premake5"
            executable.parent.mkdir(parents=True)
            executable.touch()
            prompts = mock.Mock(spec=PromptPolicy)

            result = premake.ensure_premake(
                root, self.configuration(), prompts, system="Darwin"
            )

        self.assertEqual(result, executable)
        prompts.confirm.assert_not_called()

    @mock.patch.object(premake, "unzip_file")
    @mock.patch.object(premake, "download_file")
    def test_install_verifies_archive_and_license(self, download_file, unzip_file):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            executable = root / "vendor/premake/bin/premake5"

            def extract(_archive, delete_zip_file=True):
                executable.parent.mkdir(parents=True, exist_ok=True)
                executable.touch()

            unzip_file.side_effect = extract
            result = premake.ensure_premake(
                root,
                self.configuration(),
                PromptPolicy(assume_yes=True),
                system="Darwin",
            )

        self.assertEqual(result, executable)
        self.assertEqual(download_file.call_args_list[0].args[2], "a" * 64)
        self.assertEqual(download_file.call_args_list[1].args[2], "b" * 64)


if __name__ == "__main__":
    unittest.main()
