import sys

from mixture_tools.cli import main

if __name__ == "__main__":
    arguments = sys.argv[1:] or ["setup"]
    raise SystemExit(main(arguments))
