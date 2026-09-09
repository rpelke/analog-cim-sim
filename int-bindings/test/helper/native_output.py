##############################################################################
# Copyright (C) 2026 Joel Klein                                              #
# All Rights Reserved                                                        #
#                                                                            #
# This is work is licensed under the terms described in the LICENSE file     #
# found in the root directory of this source tree.                           #
##############################################################################
import contextlib
import os
import sys


@contextlib.contextmanager
def suppressed_native_output(stdout=False, stderr=False):
    """Silence what acs_py writes to the process file descriptors.
    """
    fds = ([1] if stdout else []) + ([2] if stderr else [])

    sys.stdout.flush()
    sys.stderr.flush()

    saved = {fd: os.dup(fd) for fd in fds}
    try:
        with open(os.devnull, "w") as devnull:
            for fd in fds:
                os.dup2(devnull.fileno(), fd)
            yield
    finally:
        for fd, backup in saved.items():
            os.dup2(backup, fd)
            os.close(backup)
