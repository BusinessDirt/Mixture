import argparse
import logging
from collections.abc import Sequence

from .context import Context
from .errors import MixtureToolsError
from .log import LogFormatter
from .prompting import PromptPolicy
from .setup.runner import run_setup
from .version import Version, read_version, write_version

def _add_setup_parser(subparsers) -> None:
    """Scope for the 'setup' command."""
    setup = subparsers.add_parser(
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

def _add_version_parser(subparsers) -> None:
    """Scope for the 'version' command and its subcommands."""
    version = subparsers.add_parser(
        "version",
        help="Read or update the engine version",
    )
    version_commands = version.add_subparsers(
        dest="version_command",
        required=True,
    )

    version_commands.add_parser("show", help="Print the current engine version")

    set_version = version_commands.add_parser("set", help="Set an explicit engine version")
    set_version.add_argument("value", type=Version.parse)

    bump_version = version_commands.add_parser("bump", help="Increment part of the engine version")
    bump_version.add_argument("part", choices=("major", "minor", "patch"))

def _add_visualizer_parser(subparsers) -> None:
    """Scope for the 'visualizer' command and its subcommands."""
    visualizer = subparsers.add_parser(
        "visualizer",
        help="Start a visualizer",
    )
    visualizer_commands = visualizer.add_subparsers(
        dest="visualizer_commands",
        required=True,
    )

    visualizer_commands.add_parser(
        "render_graph",
        help="Shows a render graph visualization (Application has to have run at least once)",
    )

def create_parser() -> argparse.ArgumentParser:
    """Main parser entry point."""
    parser = argparse.ArgumentParser(
        prog="mixture",
        description="Mixture engine development tools",
    )

    commands = parser.add_subparsers(dest="command", required=True)

    # Attach the scoped subparsers
    _add_setup_parser(commands)
    _add_version_parser(commands)
    _add_visualizer_parser(commands)

    return parser


def main(arguments: Sequence[str] | None = None) -> int:
    handler = logging.StreamHandler()
    handler.setFormatter(LogFormatter(datefmt='%H:%M:%S'))
    logging.basicConfig(level=logging.INFO, handlers=[handler])

    logger = logging.getLogger(__name__)

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
                        logger.info(current)
                        return 0

                    case "set":
                        updated = options.value

                    case "bump":
                        updated = current.bump(options.part)

                write_version(context.configuration_file, updated)
                logger.info(f"{current} -> {updated}")
                return 0

            case "visualizer":
                match options.visualizer_commands:
                    case "render_graph":
                        from .visualizers import render_graph
                        render_graph.visualize(context.repository_root, "docs/visualizers/graph.json")
                        return 0

    except MixtureToolsError as error:
        logging.getLogger("Setup").error("%s", error)
        return 1

    parser.error("Unknown command")
