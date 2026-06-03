#!/usr/bin/bash
set -euo pipefail

# Create the virtual environment if it does not exist
if [ ! -d ".venv" ]; then
    python3 -m venv .venv
fi

# Activate .venv environment
source .venv/bin/activate

# Install requirements
pip install --upgrade pip
pip install -r requirements.txt
