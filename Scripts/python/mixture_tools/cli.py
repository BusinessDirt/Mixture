import argparse
import logging
from collections.abc import Sequence

from .context import Context
from .errors import MixtureToolsError
from .prompting import PromptPolicy
from .setup.runner import run_setup
from .version import Version, read_version, write_version

class LogFormatter(logging.Formatter):
    # ANSI Escape Codes
    grey = "\x1b[38;20m"
    blue = "\x1b[34;20m"
    green = "\x1b[32;20m"
    yellow = "\x1b[33;20m"
    red = "\x1b[31;20m"
    bold_red = "\x1b[31;1m"
    bold_pink = "\x1b[1;35m" # For logger name (markers)
    reset = "\x1b[0m"

    LEVEL_COLORS = {
        logging.DEBUG: blue,
        logging.INFO: green,
        logging.WARNING: yellow,
        logging.ERROR: red,
        logging.CRITICAL: bold_red
    }

    def format(self, record):
        # Ensure time is formatted
        record.asctime = self.formatTime(record, self.datefmt)

        # Determine colors
        level_color = self.LEVEL_COLORS.get(record.levelno, self.grey)

        # Format Logger Name (Base.Marker)
        # "marker only things after the . should be marked"
        name = record.name
        if "." in name:
            base, marker = name.split(".", 1)
            # Base (Default) + .Marker (Pink)
            fmt_name = f"{base}{self.bold_pink}/{marker}{self.reset}"
        else:
            fmt_name = name

        # Format Message
        message = record.getMessage()

        log_str = (
            f"[{record.asctime}] "
            f"[{record.threadName}/{level_color}{record.levelname}{self.reset}] "
            f"({fmt_name}): "
            f"{level_color}{message}{self.reset}"
        )

        return log_str


def create_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="mixture",
        description="Mixture engine development tools",
    )

    commands = parser.add_subparsers(dest="command", required=True)

    setup = commands.add_parser(
        "setup",
        help="Validate dependencies and generate project files",
    )
    setup.add_argument(
        "-y", "--yes", action="store_true",
        help="Automatically approve downloads and installations",
    )
    setup.add_argument(
        "--non-interactive", action="store_true",
        help="Fail instead of requesting user input",
    )

    version = commands.add_parser(
        "version",
        help="Read or update the engine version",
    )

    version_commands = version.add_subparsers(
        dest="version_command",
        required=True,
    )

    version_commands.add_parser(
        "show",
        help="Print the current engine version",
    )

    set_version = version_commands.add_parser(
        "set",
        help="Set an explicit engine version",
    )
    set_version.add_argument("value", type=Version.parse)

    bump_version = version_commands.add_parser(
        "bump",
        help="Increment part of the engine version",
    )
    bump_version.add_argument(
        "part",
        choices=("major", "minor", "patch"),
    )

    return parser


def main(arguments: Sequence[str] | None = None) -> int:
    handler = logging.StreamHandler()
    handler.setFormatter(LogFormatter(datefmt='%H:%M:%S'))
    logging.basicConfig(level=logging.INFO, handlers=[handler])

    parser = create_parser()
    options = parser.parse_args(arguments)
    try:
        context = Context.discover()

        match options.command:
            case "setup":
                prompts = PromptPolicy.from_environment(
                    assume_yes=options.yes,
                    interactive=not options.non_interactive,
                )
                return run_setup(context, prompts)

            case "version":
                current = read_version(context.configuration_file)

                match options.version_command:
                    case "show":
                        print(current)
                        return 0

                    case "set":
                        updated = options.value

                    case "bump":
                        updated = current.bump(options.part)

                write_version(context.configuration_file, updated)
                print(f"{current} -> {updated}")
                return 0
    except MixtureToolsError as error:
        logging.getLogger("Setup").error("%s", error)
        return 1

    parser.error("Unknown command")
