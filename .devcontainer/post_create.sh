#!/usr/bin/bash
set -euo pipefail

# Make sure the submodules are present
git submodule update --init --recursive

# A .venv built on the host points at a Python that does not exist here.
# Test that it still works instead of comparing versions: .venv/bin/python is
# a relative symlink and resolves fine while the rest of the venv is stale.
if [ -d ".venv" ] && ! .venv/bin/python -m pip --version >/dev/null 2>&1; then
    echo "Removing unusable .venv, rebuilding it for $(python3 --version)"
    rm -rf .venv
fi

# Create the virtual environment if it does not exist
if [ ! -d ".venv" ]; then
    python3 -m venv .venv
fi

# Likewise, a CMake cache records absolute paths, so a build tree configured
# on the host is unusable here. Drop only caches that moved.
if [ -d "build" ]; then
    while IFS= read -r cache; do
        cache_dir="$(dirname "${cache}")"
        # CMAKE_CACHEFILE_DIR is absolute, so compare it to the directory the
        # cache actually sits in (-ef: same directory, however it is spelled).
        cache_dir_rec="$(sed -n 's/^CMAKE_CACHEFILE_DIR:INTERNAL=//p' "${cache}")"
        if [ -z "${cache_dir_rec}" ] || ! [ "${cache_dir_rec}" -ef "${cache_dir}" ]; then
            echo "Removing stale CMake build tree ${cache_dir}"
            rm -rf "${cache_dir}"
        fi
    done < <(find build -name CMakeCache.txt)
fi

# Activate .venv environment
# shellcheck source=/dev/null
source .venv/bin/activate

# Install requirements
pip install --upgrade pip
pip install -r requirements.txt

# Install the style tooling (yapf, pymarkdown) used by ./util/format_*.py
pip install -r util/requirements_style.txt
