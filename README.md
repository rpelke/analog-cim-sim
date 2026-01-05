# Analog-CIM-Sim
[![CMake](https://github.com/rpelke/analog-cim-sim/actions/workflows/cmake.yml/badge.svg)](https://github.com/rpelke/analog-cim-sim/actions/workflows/cmake.yml)
[![Style](https://github.com/rpelke/analog-cim-sim/actions/workflows/style.yml/badge.svg)](https://github.com/rpelke/analog-cim-sim/actions/workflows/style.yml)
[![Python](https://github.com/rpelke/analog-cim-sim/actions/workflows/python.yml/badge.svg)](https://github.com/rpelke/analog-cim-sim/actions/workflows/python.yml)
![Coverage (lines)](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/rpelke/4ce01f0a4277ab79baa3b2112b12812f/raw)

Yet another simulator for executing matrix-vector multiplications on analog computing-in-memory crossbars.

If you use the code of this repository, please consider citing the corresponding [paper](https://arxiv.org/abs/2505.14303):
```
@misc{pelke2025optimizingbinaryternaryneural,
    title={{Optimizing Binary and Ternary Neural Network Inference on RRAM Crossbars using CIM-Explorer}},
    author={Rebecca Pelke and José Cubero-Cascante and Nils Bosbach and Niklas Degener and Florian Idrizi and Lennart M. Reimann and Jan Moritz Joseph and Rainer Leupers},
    year={2025},
    eprint={2505.14303},
    archivePrefix={arXiv},
    primaryClass={cs.ET},
    url={https://arxiv.org/abs/2505.14303},
}
```
This simulator is used by [CIM-E](https://github.com/rpelke/CIM-E), a design space exploration tool for neural networks.

## Build instructions

### Requirements
To build the project, you will require:
- `cmake` >= 3.15
- `python3` (*dev version* for pybind11)
- oneAPI Threading Building Blocks (`oneTBB`). On Debian-based distributions, `sudo apt install libtbb-dev`. For more details see [here](https://uxlfoundation.github.io/oneTBB/index.html).

### Building
Clone the repository including submodules:
```bash
git clone --recursive git@github.com:rpelke/analog-cim-sim.git
```

Create a virtual environment:
```bash
python3 -m venv .venv
source .venv/bin/activate
pip3 install -r requirements.txt
```

Build and install the project (replace the placeholders):
```bash
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
