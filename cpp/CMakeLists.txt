##############################################################################
# Copyright (C) 2025 Rebecca Pelke                                           #
# All Rights Reserved                                                        #
#                                                                            #
# This is work is licensed under the terms described in the LICENSE file     #
# found in the root directory of this source tree.                           #
##############################################################################
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Code coverage
option(COVERAGE "Enable code coverage." OFF)
message(STATUS "Coverage: ${COVERAGE}")

# Install path for python package
if(NOT DEFINED PY_INSTALL_PATH)
    message(FATAL_ERROR "PY_INSTALL_PATH is not set! Please define it with -DPY_INSTALL_PATH=<...>")
endif()

# Use gcc for coverage
if (COVERAGE)
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
endif()

project(analog-cim-sim)

# Include json
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(extern/json EXCLUDE_FROM_ALL)

# Test options
option(LIB_TESTS "Build lib tests." OFF)

# Build lib options
option(BUILD_LIB_ACS_BNNTNN "Build bnn tnn lib." OFF)
option(BUILD_LIB_ACS_INT "Build int lib." OFF)

if (COVERAGE)
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(FATAL_ERROR "Error: The build type must be 'Debug'!")
    endif()
endif()

### Build BNN/TNN library ###
if(BUILD_LIB_ACS_BNNTNN)
    message(STATUS "Build LIB_ACS_BNNTNN.")

    set(SOURCES_BNN_TNN
        src/interface_bnn_tnn.cpp
    )
    add_library(acs_bnntnn SHARED ${SOURCES_BNN_TNN})
    if (COVERAGE)
        target_compile_options(acs_bnntnn PRIVATE -fprofile-arcs -ftest-coverage)
        target_link_libraries(acs_bnntnn PRIVATE gcov)
    endif()
    set_target_properties(acs_bnntnn PROPERTIES POSITION_INDEPENDENT_CODE ON)
    install(TARGETS acs_bnntnn DESTINATION lib)
endif()


### Build Int library ###
if(BUILD_LIB_ACS_INT)
    message(STATUS "Build LIB_ACS_INT.")
    find_package(Python3 COMPONENTS Interpreter Development)
    execute_process(
        COMMAND ${Python3_EXECUTABLE} -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')"
        OUTPUT_VARIABLE PYTHON_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    message(STATUS "Use Python: ${PYTHON_VERSION}")
    find_package(pybind11 CONFIG REQUIRED PATHS ${CMAKE_CURRENT_LIST_DIR}/../.venv/lib/python${PYTHON_VERSION}/site-packages/pybind11/share/cmake/pybind11)

    set(SOURCES_INT
        src/interface_int.cpp
        src/helper/config.cpp
        src/mapping/mapper.cpp
        src/mapping/mapperA.cpp
        src/mapping/mapperB.cpp
        src/mapping/mapperC.cpp
        src/mapping/mapperD.cpp
        src/mapping/mapperE.cpp
        src/xbar/crossbar.cpp
        src/adc/adc.cpp
        src/adc/adcfactory.cpp
        src/adc/symadc.cpp
        src/adc/posadc.cpp
        src/adc/infadc.cpp
    )

    # Build pybind module
    pybind11_add_module(acs_int SHARED ${SOURCES_INT})
    include_directories(inc extern/json/include)
    target_link_libraries(acs_int PRIVATE nlohmann_json::nlohmann_json pybind11::module)
    if (COVERAGE)
        target_compile_options(acs_int PRIVATE -fprofile-arcs -ftest-coverage)
        target_link_libraries(acs_int PRIVATE gcov)
    endif()
    set_target_properties(acs_int PROPERTIES POSITION_INDEPENDENT_CODE ON)
    install(TARGETS acs_int DESTINATION ${PY_INSTALL_PATH})
endif()


if(LIB_TESTS)
    message(STATUS "Enabling cpp tests.")
    enable_testing()
    add_subdirectory(test)
endif()
