import sys
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from mixture_tools.errors import PermissionDeniedError
from mixture_tools.prompting import PromptPolicy


class PromptPolicyTests(unittest.TestCase):
    def test_assume_yes_does_not_prompt(self):
        with mock.patch("builtins.input") as user_input:
            self.assertTrue(PromptPolicy(assume_yes=True).confirm("Continue?"))
        user_input.assert_not_called()

    def test_non_interactive_mode_fails_when_approval_is_required(self):
        with self.assertRaises(PermissionDeniedError):
            PromptPolicy(interactive=False).confirm("Continue?")

    def test_reprompts_until_answer_is_valid(self):
        with mock.patch("builtins.input", side_effect=["maybe", "y"]):
            self.assertTrue(PromptPolicy().confirm("Continue?"))


if __name__ == "__main__":
    unittest.main()
