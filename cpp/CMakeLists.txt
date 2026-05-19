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

include(GNUInstallDirs)
# Include json
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(extern/json EXCLUDE_FROM_ALL)

# Test options
option(LIB_TESTS "Build lib tests." OFF)

# Debug mode for matrix operations
option(DEBUG_MODE "Enable debug output for matrix operations" OFF)

# Build lib options
option(BUILD_LIB_CB_EMU "Build emulater/callback interface." OFF)
option(BUILD_LIB_ACS_INT "Build int lib." OFF)
option(BUILD_LIB_ACS_CPP "Build cpp lib." OFF)

if (COVERAGE)
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
message(FATAL_ERROR "Error: The build type must be 'Debug'!")
endif()
endif()

### Build emulater/callback interface ###
if(BUILD_LIB_CB_EMU)
    message(STATUS "Build emulater/callback interface.")
    
    set(SOURCES_CB
    src/interface_cb.cpp
    )
    add_library(acs_cb_emu SHARED ${SOURCES_CB})
    if (COVERAGE)
    target_compile_options(acs_cb_emu PRIVATE -fprofile-arcs -ftest-coverage)
    target_link_libraries(acs_cb_emu PRIVATE gcov)
    endif()
    set_target_properties(acs_cb_emu PROPERTIES POSITION_INDEPENDENT_CODE ON)
    install(TARGETS acs_cb_emu DESTINATION lib)
    endif()
    
    
    ### Build Int library ###
    if(BUILD_LIB_ACS_INT)

    # Install path for python package
    if(NOT DEFINED PY_INSTALL_PATH)
    message(FATAL_ERROR "PY_INSTALL_PATH is not set! Please define it with -DPY_INSTALL_PATH=<...>")
    endif()

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
        src/interface_xbar.cpp
        src/interface_pybind.cpp
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

    
    # Build pybind module
    pybind11_add_module(acs_int SHARED ${SOURCES_INT})
    include_directories(inc extern/json/include)
    target_link_libraries(acs_int PRIVATE nlohmann_json::nlohmann_json pybind11::module)

    # Build with test coverage
    if (COVERAGE)
        target_compile_options(acs_int PRIVATE -fprofile-arcs -ftest-coverage)
        target_link_libraries(acs_int PRIVATE gcov)
    endif()

    # Link with oneTBB (Thread Building Blocks)
    find_package(TBB REQUIRED)
    message(STATUS "Found TBB: (found version \"${TBB_VERSION}\"), linking acs_int against TBB (Parallel STL backend).")
    target_link_libraries(acs_int PRIVATE TBB::tbb)

    set_target_properties(acs_int PROPERTIES POSITION_INDEPENDENT_CODE ON)
    install(TARGETS acs_int DESTINATION ${PY_INSTALL_PATH})
endif()

if(BUILD_LIB_ACS_CPP)
    message(STATUS "Build LIB_ACS_CPP.")

    set(SOURCES_CPP
        src/interface_xbar.cpp
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

    set(HEADERS
        inc/helper/definitions.h
        inc/helper/config.h
        inc/helper/histogram.h
        inc/xbar/parasitics.h
        inc/xbar/adc.h
        inc/xbar/crossbar.h
        inc/xbar/read_disturb.h
        inc/interface_xbar.h
        inc/global_vars.h
        inc/mapping/tnn_mapper/tnn_iv.h
        inc/mapping/tnn_mapper/tnn_v.h
        inc/mapping/tnn_mapper/tnn_ii.h
        inc/mapping/tnn_mapper/tnn_iii.h
        inc/mapping/tnn_mapper/tnn_i.h
        inc/mapping/int_mapper/int_iii.h
        inc/mapping/int_mapper/int_iv.h
        inc/mapping/int_mapper/int_v.h
        inc/mapping/int_mapper/int_ii.h
        inc/mapping/int_mapper/int_i.h
        inc/mapping/mapper.h
        inc/mapping/bnn_mapper/bnn_vi.h
        inc/mapping/bnn_mapper/bnn_ii.h
        inc/mapping/bnn_mapper/bnn_v.h
        inc/mapping/bnn_mapper/bnn_iii.h
        inc/mapping/bnn_mapper/bnn_i.h
        inc/mapping/bnn_mapper/bnn_iv.h
    )

    add_library(acs_cpp SHARED ${SOURCES_CPP} ${HEADERS})
    # Build json library
    include_directories(inc extern/json/include)
    target_link_libraries(acs_cpp PRIVATE nlohmann_json::nlohmann_json)

    # Build with test coverage
    if (COVERAGE)
        target_compile_options(acs_cpp PRIVATE -fprofile-arcs -ftest-coverage)
        target_link_libraries(acs_cpp PRIVATE gcov)
    endif()

    # Link with oneTBB (Thread Building Blocks)
    find_package(TBB REQUIRED)
    message(STATUS "Found TBB: (found version \"${TBB_VERSION}\"), linking acs_cpp against TBB (Parallel STL backend).")
    target_link_libraries(acs_cpp PRIVATE TBB::tbb)

    set_target_properties(acs_cpp PROPERTIES POSITION_INDEPENDENT_CODE ON)
    set_target_properties(acs_cpp PROPERTIES PUBLIC_HEADER inc/interface_xbar.h)
    install(TARGETS acs_cpp
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

endif()


if(LIB_TESTS)
    message(STATUS "Enabling cpp tests.")
    enable_testing()
    add_subdirectory(test)
endif()

if(DEBUG_MODE)
    add_definitions(-DDEBUG_MODE)
    message(STATUS "Debug mode enabled - will print matrix operation details")
endif()
