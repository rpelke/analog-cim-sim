#!/usr/bin/bash

# can be found in .venv/lib/
export PY_PACKAGE_DIR=/workspaces/analog-cim-sim/.venv/lib/python3.12 

cd /workspaces/analog-cim-sim/build/release/build

# Build the project
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DPY_INSTALL_PATH=${PY_PACKAGE_DIR}/site-packages \
    -DCMAKE_PREFIX_PATH=${PY_PACKAGE_DIR}/pybind11/share/cmake/pybind11 \
    -DCMAKE_INSTALL_PREFIX=../ \
    -DLIB_TESTS=ON \
    -DBUILD_LIB_CB_EMU=ON \
    -DBUILD_LIB_ACS_INT=ON \
    -DBUILD_LIB_ACS_CPP=ON \
    -DDEBUG_MODE=OFF \
    ../../../cpp

make -j "$(nproc)"
make install