import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from mixture_tools.errors import ProcessError
from mixture_tools.process import run


class ProcessTests(unittest.TestCase):
    def test_returns_captured_process_result(self):
        with tempfile.TemporaryDirectory() as directory, mock.patch(
            "mixture_tools.process.subprocess.run"
        ) as subprocess_run:
            subprocess_run.return_value = subprocess.CompletedProcess(
                args=("tool",), returncode=0, stdout="output", stderr=""
            )

            result = run(["tool", Path("argument")], cwd=Path(directory))

        self.assertEqual(result.command, ("tool", "argument"))
        self.assertEqual(result.stdout, "output")
        self.assertFalse(subprocess_run.call_args.kwargs["check"])

    def test_raises_typed_error_for_failed_command(self):
        with tempfile.TemporaryDirectory() as directory, mock.patch(
            "mixture_tools.process.subprocess.run"
        ) as subprocess_run:
            subprocess_run.return_value = subprocess.CompletedProcess(
                args=("tool",), returncode=7, stdout="", stderr="failed"
            )

            with self.assertRaises(ProcessError) as raised:
                run(["tool"], cwd=Path(directory))

        self.assertEqual(raised.exception.return_code, 7)


if __name__ == "__main__":
    unittest.main()
