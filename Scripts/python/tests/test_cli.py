import io
import sys
import tempfile
import unittest
from contextlib import redirect_stdout
from pathlib import Path
from types import SimpleNamespace
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from mixture_tools import cli


class CliTests(unittest.TestCase):
    def test_version_show_prints_configured_version(self):
        with tempfile.TemporaryDirectory() as directory:
            configuration = Path(directory) / "mixture.toml"
            configuration.write_text('[engine]\nversion = "1.2.3"\n', encoding="utf-8")
            context = SimpleNamespace(configuration_file=configuration)
            output = io.StringIO()

            with mock.patch.object(cli.Context, "discover", return_value=context), redirect_stdout(output):
                result = cli.main(["version", "show"])

        self.assertEqual(result, 0)
        self.assertEqual(output.getvalue().strip(), "1.2.3")

    def test_setup_passes_non_interactive_prompt_policy(self):
        context = SimpleNamespace()
        with mock.patch.object(cli.Context, "discover", return_value=context), mock.patch.object(
            cli, "run_setup", return_value=0
        ) as run_setup, mock.patch.dict("os.environ", {}, clear=True):
            result = cli.main(["setup", "--non-interactive"])

        self.assertEqual(result, 0)
        prompts = run_setup.call_args.args[1]
        self.assertFalse(prompts.interactive)
        self.assertFalse(prompts.assume_yes)

    def test_setup_auto_approves_in_ci(self):
        context = SimpleNamespace()
        with mock.patch.object(cli.Context, "discover", return_value=context), mock.patch.object(
            cli, "run_setup", return_value=0
        ) as run_setup, mock.patch.dict("os.environ", {"CI": "true"}, clear=True):
            result = cli.main(["setup", "--non-interactive"])

        self.assertEqual(result, 0)
        prompts = run_setup.call_args.args[1]
        self.assertFalse(prompts.interactive)
        self.assertTrue(prompts.assume_yes)


if __name__ == "__main__":
    unittest.main()
