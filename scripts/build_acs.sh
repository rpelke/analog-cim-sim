#!/usr/bin/bash
set -euo pipefail

# Dynamically locate the project root directory
SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)
PROJECT_ROOT=$(dirname "${SCRIPT_DIR}")

BUILD_TYPE="Debug"

usage() {
    cat <<EOF
Usage: ${0##*/} [-t|--type <type>] [-h|--help]

Configure, build and install the C++ part of analog-cim-sim.

Options:
  -t, --type <type>  CMake build type: Debug (default), Release,
                     RelWithDebInfo or MinSizeRel. Case insensitive.
  -h, --help         Show this help and exit.

The build tree is <repo>/build/<lower case type>/build and the libraries and
headers are installed into <repo>/build/<lower case type>. Coverage
instrumentation is only added to the Debug build: cpp/CMakeLists.txt rejects
-DCOVERAGE=ON for every other build type.
EOF
}

while [ $# -gt 0 ]; do
    case "$1" in
        -t | --type)
            if [ $# -lt 2 ]; then
                echo "Error: $1 requires an argument." >&2
                exit 2
            fi
            BUILD_TYPE="$2"
            shift 2
            ;;
        --type=*)
            BUILD_TYPE="${1#*=}"
            shift
            ;;
        -h | --help)
            usage
            exit 0
            ;;
        *)
            echo "Error: unknown argument '$1'." >&2
            usage >&2
            exit 2
            ;;
    esac
done

# Accept any spelling, continue with the one CMake and the directory expect
case "${BUILD_TYPE,,}" in
    debug) BUILD_TYPE="Debug" ;;
    release) BUILD_TYPE="Release" ;;
    relwithdebinfo) BUILD_TYPE="RelWithDebInfo" ;;
    minsizerel) BUILD_TYPE="MinSizeRel" ;;
    *)
        echo "Error: unknown build type '${BUILD_TYPE}'." >&2
        echo "Use Debug, Release, RelWithDebInfo or MinSizeRel." >&2
        exit 2
        ;;
esac

# Coverage requires gcov instrumentation, which only the Debug build carries
COVERAGE="OFF"
if [ "${BUILD_TYPE}" = "Debug" ]; then
    COVERAGE="ON"
fi

BUILD_TYPE_DIR="${BUILD_TYPE,,}"
BUILD_DIR="${PROJECT_ROOT}/build/${BUILD_TYPE_DIR}/build"
INSTALL_DIR="${PROJECT_ROOT}/build/${BUILD_TYPE_DIR}"

# Dynamically detect python version inside the .venv
PYTHON_VERSION=$(python3 -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')")

# Can be found in .venv/lib/
export PY_PACKAGE_DIR="${PROJECT_ROOT}/.venv/lib/python${PYTHON_VERSION}"

echo "Build type:   ${BUILD_TYPE} (coverage: ${COVERAGE})"
echo "Build tree:   ${BUILD_DIR}"
echo "Install into: ${INSTALL_DIR}"

mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Build the project
cmake \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DPY_INSTALL_PATH="${PY_PACKAGE_DIR}/site-packages" \
    -DCMAKE_PREFIX_PATH="${PY_PACKAGE_DIR}/pybind11/share/cmake/pybind11" \
    -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" \
    -DLIB_TESTS=ON \
    -DCOVERAGE="${COVERAGE}" \
    -DBUILD_LIB_CB_EMU=ON \
    -DBUILD_LIB_ACS_PY=ON \
    -DBUILD_LIB_ACS_CORE=ON \
    -DDEBUG_MODE=OFF \
    "${PROJECT_ROOT}/cpp"

make -j "$(nproc)"
make install

# acs_py is installed into the .venv
echo "Installed acs_py into ${PY_PACKAGE_DIR}/site-packages."
