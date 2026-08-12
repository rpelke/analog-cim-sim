#!/usr/bin/env python3
##############################################################################
# Copyright (C) 2025 Joel Klein                                              #
# All Rights Reserved                                                        #
#                                                                            #
# This is work is licensed under the terms described in the LICENSE file     #
# found in the root directory of this source tree.                           #
##############################################################################
import re
import subprocess
import sys
from pathlib import Path

proj_dir = Path(__file__).resolve().parents[1]
shfmt_args = ["-i", "4", "-ci"]
shebang = re.compile(rb"^#!.*\b(bash|sh|dash|ksh)\b")


def is_shell_file(path):
    """Shell files are recognized by extension or, if there is none, shebang."""
    if path.suffix in (".sh", ".bash"):
        return True
    if path.suffix != "":
        return False
    try:
        with path.open("rb") as f:
            return shebang.match(f.readline()) is not None
    except OSError:
        return False


# Ask git which files belong to the repository. That skips ignored paths
# (.venv, build) and submodules (googletest, json) without an exclude list,
# while still covering new files that are not committed yet.
tracked = subprocess.run(["git", "ls-files", "--cached", "--others", "--exclude-standard"],
                         cwd=proj_dir,
                         capture_output=True,
                         text=True,
                         check=True)

files = sorted(
    str(proj_dir / f) for f in tracked.stdout.splitlines()
    if (proj_dir / f).is_file() and is_shell_file(proj_dir / f))

if not files:
    sys.exit(0)

fmt = subprocess.run(["shfmt", *shfmt_args, "--diff", *files])
lint = subprocess.run(["shellcheck", *files])

sys.exit(fmt.returncode or lint.returncode)
