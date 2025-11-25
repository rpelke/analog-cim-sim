##############################################################################
# Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan                   #
# All Rights Reserved                                                        #
#                                                                            #
# This is work is licensed under the terms described in the LICENSE file     #
# found in the root directory of this source tree.                           #
##############################################################################
add_subdirectory(googletest EXCLUDE_FROM_ALL)

get_target_property(acs_int_path acs_int BINARY_DIR)
set(ld_library_path "${acs_int_path}")
message(STATUS "Use lib: ${ld_library_path}")

option(USE_STDCXXFS "Use stdc++fs for older GCC versions" OFF)

function(add_library_test TEST_NAME SOURCE_FILE CFG_DIR)
    include_directories(inc)
    add_executable(${TEST_NAME} ${SOURCE_FILE})
    add_dependencies(${TEST_NAME} acs_int)
    target_link_libraries(${TEST_NAME} PRIVATE
        acs_int
        gtest
        gtest_main
        gmock
        dl
    )

    if (USE_STDCXXFS)
        target_link_libraries(${TEST_NAME} PRIVATE stdc++fs)
    endif()

    enable_testing()
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    set_tests_properties(${TEST_NAME}
        PROPERTIES ENVIRONMENT
        "LD_LIBRARY_PATH=${ld_library_path}:$ENV{LD_LIBRARY_PATH};CFG_DIR_TESTS=${CFG_DIR}"
    )
endfunction()

add_library_test(load_lib_tests lib/load_library_tests.cpp ${CMAKE_CURRENT_SOURCE_DIR}/lib/configs)
add_library_test(bnn_tnn_tests lib/lib_cb_emu_tests.cpp ${CMAKE_CURRENT_SOURCE_DIR}/lib/configs)
add_library_test(mvm_tests lib/mvm_tests.cpp ${CMAKE_CURRENT_SOURCE_DIR}/lib/configs)
add_library_test(var_tests lib/var_tests.cpp ${CMAKE_CURRENT_SOURCE_DIR}/lib/configs)
add_library_test(adc_tests lib/adc_tests.cpp ${CMAKE_CURRENT_SOURCE_DIR}/lib/configs)
add_library_test(parasitics_tests lib/parasitics_tests.cpp ${CMAKE_CURRENT_SOURCE_DIR}/lib/configs)
