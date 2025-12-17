#!/bin/bash
NO_PAUSE=false

# Check arguments for a flag (case insensitive)
for arg in "$@"
do
    if [[ "${arg,,}" == "-nopause" ]] || [[ "${arg,,}" == "--nopause" ]]; then
        NO_PAUSE=true
    fi
done

# Get the directory where this script is actually saved
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PYTHON_DIR="$SCRIPT_DIR/python"

# Navigate to the directory
if pushd "$PYTHON_DIR" > /dev/null; then

    if [ -f "Setup.py" ]; then
        python3 Setup.py
    else
        echo -e "\033[0;31mError: Setup.py not found in $PYTHON_DIR\033[0m" >&2
    fi

    # Return to previous directory
    popd > /dev/null

else
    echo -e "\033[0;31mError: Could not find directory $PYTHON_DIR\033[0m" >&2
fi

# Pause logic
if [ "$NO_PAUSE" = false ]; then
    echo ""
    read -n 1 -s -r -p "Press any key to continue..."
    echo ""
fi
