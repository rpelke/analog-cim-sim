##############################################################################
# Copyright (C) 2025 Rebecca Pelke                                           #
# All Rights Reserved                                                        #
#                                                                            #
# This is work is licensed under the terms described in the LICENSE file     #
# found in the root directory of this source tree.                           #
##############################################################################
add_subdirectory(googletest EXCLUDE_FROM_ALL)

get_target_property(bnn_tnn_lib_path acs_bnntnn BINARY_DIR)
set(ld_library_path "${bnn_tnn_lib_path}")

function(add_library_test TEST_NAME SOURCE_FILE CFG_DIR)
    include_directories(inc)
    add_executable(${TEST_NAME} ${SOURCE_FILE})
    target_link_libraries(${TEST_NAME}
        gtest
        gtest_main
        dl
    )
    enable_testing()
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    set_tests_properties(${TEST_NAME}
        PROPERTIES ENVIRONMENT
        "LD_LIBRARY_PATH=${ld_library_path}:$ENV{LD_LIBRARY_PATH};CFG_DIR_TESTS=${CFG_DIR}"
    )
endfunction()

add_library_test(load_lib_tests lib/load_library_tests.cpp ${CMAKE_CURRENT_SOURCE_DIR}/lib/configs)
add_library_test(bnn_tnn_tests lib/bnn_tnn_tests.cpp ${CMAKE_CURRENT_SOURCE_DIR}/lib/configs)
add_library_test(int_tests lib/int_tests.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../../int-bindings/test/configs)
