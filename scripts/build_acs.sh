#!/usr/bin/bash

# Dynamically locate the project root directory
SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
PROJECT_ROOT=$(dirname "$SCRIPT_DIR")

# Dynamically detect python version inside the .venv
PYTHON_VERSION=$(python3 -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')")

# Can be found in .venv/lib/
export PY_PACKAGE_DIR="${PROJECT_ROOT}/.venv/lib/python${PYTHON_VERSION}"

mkdir -p "${PROJECT_ROOT}/build/debug/build"
cd "${PROJECT_ROOT}/build/debug/build"

# Build the project
cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DPY_INSTALL_PATH=${PY_PACKAGE_DIR}/site-packages \
    -DCMAKE_PREFIX_PATH=${PY_PACKAGE_DIR}/pybind11/share/cmake/pybind11 \
    -DCMAKE_INSTALL_PREFIX=../ \
    -DLIB_TESTS=ON \
    -DCOVERAGE=ON \
    -DBUILD_LIB_CB_EMU=ON \
    -DBUILD_LIB_ACS_INT=ON \
    -DBUILD_LIB_ACS_CPP=ON \
    -DDEBUG_MODE=OFF \
    ../../../cpp

make -j "$(nproc)"
make install
