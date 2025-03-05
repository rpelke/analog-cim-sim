# Analog-CIM-Sim
[![CMake](https://github.com/rpelke/analog-cim-sim/actions/workflows/cmake.yml/badge.svg)](https://github.com/rpelke/analog-cim-sim/actions/workflows/cmake.yml)
[![Style](https://github.com/rpelke/analog-cim-sim/actions/workflows/style.yml/badge.svg)](https://github.com/rpelke/analog-cim-sim/actions/workflows/style.yml)
[![Python](https://github.com/rpelke/analog-cim-sim/actions/workflows/python.yml/badge.svg)](https://github.com/rpelke/analog-cim-sim/actions/workflows/python.yml)
![Coverage (lines)](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/rpelke/4ce01f0a4277ab79baa3b2112b12812f/raw)

Yet another simulator for executing matrix-vector multiplications on analog computing-in-memory crossbars.

## Build Instructions
To build the project, you need cmake and a *dev version* of python.
If you don't have python3-dev, pybind won't compile.

Execute the following steps (replace the placeholders):
```bash
python3 -m venv .venv
source .venv/bin/activate
pip3 install -r requirements.txt

mkdir -p build/release/build && cd build/release/build

# <proj-dir> should be an absolute path to the analog-cim-sim repository
# <python-version> can be found in your .venv, e.g., python3.8
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DPY_INSTALL_PATH=<proj-dir>/.venv/lib/<python-version>/site-packages \
    -DCMAKE_PREFIX_PATH=<proj-dir>/.venv/lib/<python-version>/site-packages/pybind11/share/cmake/pybind11 \
    -DCMAKE_INSTALL_PREFIX=../ \
    -DLIB_TESTS=ON \
    -DBUILD_LIB_CB_EMU=ON \
    -DBUILD_LIB_ACS_INT=ON \
    ../../../cpp

make -j `nproc`
make install
```

Execute the tests:
```Python
python3 -m unittest discover -s int-bindings/test -p '*_test.py'
```
