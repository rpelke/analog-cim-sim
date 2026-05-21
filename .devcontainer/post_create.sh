#!/usr/bin/bash
set -euo pipefail

# Activate .venv environment
source /workspaces/analog-cim-sim/.venv/bin/activate

# Install requirements
pip install -r requirements.txt
