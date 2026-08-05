##############################################################################
# Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan                   #
# All Rights Reserved                                                        #
#                                                                            #
# This is work is licensed under the terms described in the LICENSE file     #
# found in the root directory of this source tree.                           #
##############################################################################
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Code coverage
option(COVERAGE "Enable code coverage." OFF)
message(STATUS "Coverage: ${COVERAGE}")

# Use gcc for coverage
if (COVERAGE)
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
endif()

project(analog-cim-sim)

# Include for standardized install directories
include(GNUInstallDirs)

# Include json
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(extern/json EXCLUDE_FROM_ALL)

# Test options
option(LIB_TESTS "Build lib tests." OFF)

# Debug mode for matrix operations
option(DEBUG_MODE "Enable debug output for matrix operations." OFF)

# Build lib options
option(BUILD_LIB_CB_EMU "Build emulater/callback interface." OFF)
option(BUILD_LIB_ACS_PY "Build Python binding lib." OFF)
option(BUILD_LIB_ACS_CORE "Build core C++ lib." OFF)

# Coverage can only be enabled if the debug build type is used
if (COVERAGE)
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(FATAL_ERROR "Error: The build type must be 'Debug'!")
    endif()
endif()

# The tests require the acs_py library
if (LIB_TESTS AND NOT BUILD_LIB_ACS_PY)
    message(FATAL_ERROR "Error: LIB_TESTS option requires BUILD_LIB_ACS_PY to be ON!")
endif()

# Set sources
set(ACS_EMU_SRC
  src/interface_emu.cpp
)

set(ACS_CORE_SRC
  src/helper/config.cpp
  src/helper/histogram.cpp
  src/mapping/mapper.cpp
  src/mapping/int_mapper/int_i.cpp
  src/mapping/int_mapper/int_ii.cpp
  src/mapping/int_mapper/int_iii.cpp
  src/mapping/int_mapper/int_iv.cpp
  src/mapping/int_mapper/int_v.cpp
  src/mapping/bnn_mapper/bnn_i.cpp
  src/mapping/bnn_mapper/bnn_ii.cpp
  src/mapping/bnn_mapper/bnn_iii.cpp
  src/mapping/bnn_mapper/bnn_iv.cpp
  src/mapping/bnn_mapper/bnn_v.cpp
  src/mapping/bnn_mapper/bnn_vi.cpp
  src/mapping/tnn_mapper/tnn_i.cpp
  src/mapping/tnn_mapper/tnn_ii.cpp
  src/mapping/tnn_mapper/tnn_iii.cpp
  src/mapping/tnn_mapper/tnn_iv.cpp
  src/mapping/tnn_mapper/tnn_v.cpp
  src/xbar/crossbar.cpp
  src/xbar/read_disturb.cpp
  src/xbar/parasitics.cpp
  src/xbar/adc.cpp
)

set(ACS_PY_SRC
  src/interface_pybind.cpp
)

### Build emulater/callback interface ###
if(BUILD_LIB_CB_EMU)
    add_library(acs_cb_emu SHARED ${ACS_EMU_SRC})
    target_include_directories(acs_cb_emu PRIVATE inc)
endif()

### Core sources, compiled once and linked into acs_core and acs_py ###
if(BUILD_LIB_ACS_CORE OR BUILD_LIB_ACS_PY)
    find_package(TBB REQUIRED)
    message(STATUS "Found TBB: (version \"${TBB_VERSION}\"). Linking libraries against TBB (Parallel STL backend).")

    add_library(acs_core_object OBJECT ${ACS_CORE_SRC})
    target_include_directories(acs_core_object PUBLIC inc extern/json/include)
    target_link_libraries(acs_core_object PUBLIC
        nlohmann_json::nlohmann_json
        TBB::tbb
    )
endif()

### Build core C++ library ###
if(BUILD_LIB_ACS_CORE)
    add_library(acs_core SHARED)
    target_link_libraries(acs_core PUBLIC acs_core_object)
endif()

### Build Python binding library ###
if(BUILD_LIB_ACS_PY)
    if(NOT DEFINED PY_INSTALL_PATH)
        message(FATAL_ERROR "PY_INSTALL_PATH is not set! Please define it with -DPY_INSTALL_PATH=<...>")
    endif()
    find_package(Python3 COMPONENTS Interpreter Development)
    execute_process(
        COMMAND ${Python3_EXECUTABLE} -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')"
        OUTPUT_VARIABLE PYTHON_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    message(STATUS "Use Python: ${PYTHON_VERSION}")
    find_package(pybind11 CONFIG REQUIRED PATHS ${CMAKE_CURRENT_LIST_DIR}/../.venv/lib/python${PYTHON_VERSION}/site-packages/pybind11/share/cmake/pybind11)
    pybind11_add_module(acs_py SHARED ${ACS_PY_SRC})
    target_link_libraries(acs_py PUBLIC pybind11::module acs_core_object)
endif()

# Apply core compiler flags & properties to ALL enabled targets
foreach(tgt acs_cb_emu acs_py acs_core acs_core_object)
    if(TARGET ${tgt})
        # Ensure Position Independent Code is set for all shared modules
        set_target_properties(${tgt} PROPERTIES POSITION_INDEPENDENT_CODE ON)

        # Apply test coverage flags dynamically if requested
        if(COVERAGE)
            target_compile_options(${tgt} PRIVATE -fprofile-arcs -ftest-coverage)
            target_link_libraries(${tgt} PRIVATE gcov)
        endif()
    endif()
endforeach()

# Install options for acs_cb_emu and acs_core libraries
foreach(tgt acs_cb_emu acs_core)
    if(TARGET ${tgt})
        install(TARGETS ${tgt} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
    endif()
endforeach()
    
# Install options for acs_core headers
if(TARGET acs_core)
    install(DIRECTORY inc/ extern/json/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()

# Install options for acs_py library
if(TARGET acs_py)
    install(TARGETS acs_py LIBRARY DESTINATION ${PY_INSTALL_PATH})
endif()

if(LIB_TESTS)
    message(STATUS "Enabling cpp tests.")
    enable_testing()
    add_subdirectory(test)
endif()

if(DEBUG_MODE)
    add_definitions(-DDEBUG_MODE)
    message(STATUS "Debug mode enabled - will print matrix operation details.")
endif()
