# Analog-CIM-Sim
[![CMake](https://github.com/rpelke/analog-cim-sim/actions/workflows/cmake.yml/badge.svg)](https://github.com/rpelke/analog-cim-sim/actions/workflows/cmake.yml)
[![Style](https://github.com/rpelke/analog-cim-sim/actions/workflows/style.yml/badge.svg)](https://github.com/rpelke/analog-cim-sim/actions/workflows/style.yml)
[![Python](https://github.com/rpelke/analog-cim-sim/actions/workflows/python.yml/badge.svg)](https://github.com/rpelke/analog-cim-sim/actions/workflows/python.yml)
![Coverage (lines)](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/rpelke/4ce01f0a4277ab79baa3b2112b12812f/raw)

Yet another simulator for executing matrix-vector multiplications on analog computing-in-memory crossbars.

## Build instructions
To build the project, you need cmake and a *dev version* of python.
If you don't have python3-dev, pybind won't compile.

Clone the repository including submodules:
```bash
git clone --recursive git@github.com:rpelke/analog-cim-sim.git
```

Execute the following steps (replace the placeholders):
```bash
python3 -m venv .venv
source .venv/bin/activate
pip3 install -r requirements.txt

export PY_PACKAGE_DIR=<path to 'site-packages'> # can be found in .venv/lib/<python-version>

mkdir -p build/release/build && cd build/release/build
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DPY_INSTALL_PATH=${PY_PACKAGE_DIR}/site-packages \
    -DCMAKE_PREFIX_PATH=${PY_PACKAGE_DIR}/pybind11/share/cmake/pybind11 \
    -DCMAKE_INSTALL_PREFIX=../ \
    -DLIB_TESTS=ON \
    -DBUILD_LIB_CB_EMU=ON \
    -DBUILD_LIB_ACS_INT=ON \
    ../../../cpp

make -j `nproc`
make install
```

### Some useful cmake options
Build project with additional debug output:
```bash
cmake -DDEBUG_MODE=ON ...
```

Use C++17 `filesystem` features for the [unittests](cpp/test/lib/inc/test_helper.h) with old gcc versions (<9.1):
```bash
cmake -DUSE_STDCXXFS=ON ...
```

## Testing and debugging
Execute the tests:
```bash
python3 -m unittest discover -s int-bindings/test -p '*_test.py'
```

To detect segmentation faults in the C++ part, you can also run:
```bash
gdb --batch --ex="run" --ex="bt" --ex="quit" --args python3 -m unittest discover -s int-bindings/test -p '*_test.py'
```
