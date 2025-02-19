#!/usr/bin/env python3
##############################################################################
# Copyright (C) 2025 Rebecca Pelke                                           #
# All Rights Reserved                                                        #
#                                                                            #
# This is work is licensed under the terms described in the LICENSE file     #
# found in the root directory of this source tree.                           #
##############################################################################
import sys
import subprocess
from pathlib import Path

proj_dir = Path(__file__).resolve().parents[1]
formatter = "yapf"
src_dirs = [f"{proj_dir}"]
exclude = [f"{proj_dir}/.venv", "__init__.py", f"{proj_dir}/cpp"]

files = [
    f for d in src_dirs for f in Path(d).rglob('*')
    if f.is_file() and f.suffix == ".py" and all(xcl not in f._str for xcl in exclude)
]
for f in files:
    r = subprocess.run([formatter, "--diff", ".style.yapf", f"{f._str}"],
                       capture_output=True,
                       text=True)

    if r.stdout:
        print(f"Unformatted Python file: {f}")
        sys.exit(1)

sys.exit(0)
