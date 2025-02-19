##############################################################################
# Copyright (C) 2025 Rebecca Pelke                                           #
# All Rights Reserved                                                        #
#                                                                            #
# This is work is licensed under the terms described in the LICENSE file     #
# found in the root directory of this source tree.                           #
##############################################################################
cmake_minimum_required(VERSION 3.15)
project(analog-cim-sim)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include json
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(extern/json EXCLUDE_FROM_ALL)

# Test options
option(LIB_TESTS "Build lib tests." OFF)

# Build lib options
option(BUILD_LIB_ACS_BNNTNN "Build bnn tnn lib." OFF)
option(BUILD_LIB_ACS_INT "Build int lib." OFF)

### Build BNN/TNN library ###
if(BUILD_LIB_ACS_BNNTNN)
    message(STATUS "Build LIB_ACS_BNNTNN.")

    set(SOURCES_BNN_TNN
        src/interface_bnn_tnn.cpp
    )
    add_library(acs_bnntnn SHARED ${SOURCES_BNN_TNN})
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
        src/mapping/crossbar_mapping.cpp
        src/xbar/digital_crossbar.cpp
        src/xbar/analog_crossbar.cpp
        src/adc/symadc.cpp
        src/adc/infadc.cpp
    )

    # Build pybind module
    pybind11_add_module(acs_int SHARED ${SOURCES_INT})
    include_directories(inc extern/json/include)
    target_link_libraries(acs_int PRIVATE nlohmann_json::nlohmann_json pybind11::module)
    set_target_properties(acs_int PROPERTIES POSITION_INDEPENDENT_CODE ON)
    install(TARGETS acs_int DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../.venv/lib/python${PYTHON_VERSION}/site-packages)
    install(TARGETS acs_int DESTINATION lib)
endif()


if(LIB_TESTS)
    message(STATUS "Enabling cpp tests.")
    enable_testing()
    add_subdirectory(test)
endif()
