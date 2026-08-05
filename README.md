# Analog-CIM-Sim

[![CMake](https://github.com/rpelke/analog-cim-sim/actions/workflows/cmake.yml/badge.svg)](https://github.com/rpelke/analog-cim-sim/actions/workflows/cmake.yml)
[![Style](https://github.com/rpelke/analog-cim-sim/actions/workflows/style.yml/badge.svg)](https://github.com/rpelke/analog-cim-sim/actions/workflows/style.yml)
[![Python](https://github.com/rpelke/analog-cim-sim/actions/workflows/python.yml/badge.svg)](https://github.com/rpelke/analog-cim-sim/actions/workflows/python.yml)
![Coverage (lines)](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/rpelke/4ce01f0a4277ab79baa3b2112b12812f/raw)

Yet another simulator for executing matrix-vector multiplications on analog computing-in-memory crossbars.

If you use the code of this repository, please consider citing the corresponding [paper](https://arxiv.org/abs/2505.14303):

```bash
@misc{pelke2025optimizingbinaryternaryneural,
    title={{Optimizing Binary and Ternary Neural Network Inference on RRAM Crossbars using CIM-Explorer}},
    author={Rebecca Pelke and José Cubero-Cascante and Nils Bosbach and Niklas Degener and Florian Idrizi
    and Lennart M. Reimann and Jan Moritz Joseph and Rainer Leupers},
    year={2025},
    eprint={2505.14303},
    archivePrefix={arXiv},
    primaryClass={cs.ET},
    url={https://arxiv.org/abs/2505.14303},
}
```

This simulator is used by [CIM-E](https://github.com/rpelke/CIM-E), a design space exploration tool for neural networks.

## Support matrix

The simulator implements several crossbar non-idealities.
Not every non-ideality is available for every mapping.
The table below tracks which simulation detail is implemented for which crossbar configuration.

Setting `digital_only: true` bypasses the analog crossbar model completely, so none of the effects below are applied.

| Non-ideality | Config keys | INT | BNN | TNN |
| --- | --- | --- | --- | --- |
| ADC quantization + clipping | `adc_type`, `resolution` | ✅ | ✅ | ✅ |
| ADC calibration (`MAX` / `CALIB`) | `adc_calib_mode`, `adc_calib_dict` | ✅ | ✅ | ✅ |
| ADC input profiling (histograms) | `adc_profile`, `adc_profile_bin_size` | ❌ | ✅ | ✅ |
| Device-to-device (D2D) variability | `HRS_NOISE`, `LRS_NOISE`, `d2d_var` | ❌ | ✅ | ✅ |
| Cycle-to-cycle (C2C) variability | `c2c_var`, `HRS_NOISE`, `LRS_NOISE` | ❌ | ✅ | ✅ |
| Read disturb | `read_disturb`, `t_read`, `V_read` | ❌ | ✅ | ✅ |
| Read disturb mitigation (`SOFTWARE` / `CELL_BASED`) | `read_disturb_mitigation_strategy` | ❌ | ✅ | ✅ |
| Parasitic wire resistance (IR drop) | `parasitics`, `w_res`, `V_read` | ✅ (except `I_TC_W_DIFF`) | ✅ | ✅ |

All three ADC models are implemented for every mapping type.
Which one is valid for a given mapping mode follows from the mapping itself and is checked when the config is loaded,
i.e. an invalid combination aborts with an error message:
mappings that accumulate a signed column current (the differential ones) require `SYM_RANGE_ADC`,
mappings whose column current is positive-only (`I_UINT_W_OFFS`, `BNN_III`, `BNN_IV`, `BNN_V`, `TNN_IV`, `TNN_V`)
require `POS_RANGE_ONLY_ADC`. `INF_ADC` models an ideal ADC without quantization and clipping and is always allowed.

## Build instructions

Clone the repository including submodules:

```bash
git clone --recursive git@github.com:rpelke/analog-cim-sim.git
```

### Building in the devcontainer

Reopen the folder in the devcontainer, the devcontainer.json will automatically build the container.

Run the build script provided in `scripts/build_acs.sh`:

```bash
./scripts/build_acs.sh
```

### Native Building

The follwing steps have been tested with Python versions `>=3.11` and `<=3.14`.

If you are not using the devcontainer, create a virtual environment:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip3 install -r requirements.txt
```

Run the build script provided in `scripts/build_acs.sh`.
Make sure that `libtbb-dev` is installed (it may be called `tbb-devel` on other Linux distributions).
You can also change the `CMAKE_BUILD_TYPE` in this script (it is currently set to `Debug`).

```bash
./scripts/build_acs.sh
```

**Or** build and install the project without the script (replace the placeholders):

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
    -DBUILD_LIB_ACS_PY=ON \
    -DBUILD_LIB_ACS_CORE=ON \
    ../../../cpp

make -j `nproc`
make install
```

This will build all available targets in release mode with support for unittests.

### Requirements

To build the project, you will require:

- `cmake` >= 3.15
- `python3` (*dev version* for pybind11)
- oneAPI Threading Building Blocks (`oneTBB`). On Debian-based distributions,
  `sudo apt install libtbb-dev`. For more details see this [website](https://uxlfoundation.github.io/oneTBB/index.html).

If you are using the devcontainer, the requirements are already installed.

### Available building targets

| Target Name | Description | Enabled By | Installed To |
| ----------- | ----------- | ---------- | ------------ |
| `acs_cb_emu` | Emulator/callback interface library | `BUILD_LIB_CB_EMU=ON` | `lib/` |
| `acs_py` | Python binding module for the C++ library | `BUILD_LIB_ACS_PY=ON` | `${PY_INSTALL_PATH}` |
| `acs_core` | Core C++ library, no interface | `BUILD_LIB_ACS_CORE=ON` | `lib/` (library) + `include/` (headers) |

## Testing and debugging

Execute the tests:

```bash
python3 -m unittest discover -s int-bindings/test -p '*_test.py'
```

To detect segmentation faults in the C++ part, you can also run:

```bash
gdb --batch --ex="run" --ex="bt" --ex="quit" --args python3 -m unittest discover -s int-bindings/test -p '*_test.py'
```

To manually test the coverage (library was built with -DCOVERAGE=ON set):

```bash
cd build/debug/build
ctest -C . --output-on-failure
lcov --capture --directory . --output-file coverage_int.info --include '*cpp*' --exclude '*extern*'
genhtml coverage_int.info --output-directory coverage_int_html
```

The line and function coverage should be displayed at the end of the `genhtml` command.

## Linting (Style)

To test the linting locally, you need `clang-format-18`.

Install the required Python packages:

```bash
pip install -r util/requirements_style.txt
```

If the files are correctly formatted, the commands below will not produce any output.

Run:

```bash
./util/format_cpp.py
./util/format_py.py
pymarkdown scan README.md
```

### Some useful cmake options

Build project with additional debug output:

```bash
cmake -DDEBUG_MODE=ON ...
```

Build the project with support for coverage:

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DLIB_TESTS=ON -DCOVERAGE=ON ...
```

Use C++17 `filesystem` features for the [unittests](cpp/test/lib/inc/test_helper.h) with old gcc versions (<9.1):

```bash
cmake -DUSE_STDCXXFS=ON ...
```
