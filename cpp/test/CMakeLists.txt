add_subdirectory(googletest EXCLUDE_FROM_ALL)

get_target_property(bnn_tnn_lib_path BNN_TNN_Interface BINARY_DIR)
get_target_property(int_lib_path INT_Interface BINARY_DIR)
set(ld_library_path "${bnn_tnn_lib_path}:${int_lib_path}")

function(add_library_test TEST_NAME SOURCE_FILE)
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
        "LD_LIBRARY_PATH=${ld_library_path}:$ENV{LD_LIBRARY_PATH};CFG_DIR_TESTS=${CMAKE_CURRENT_SOURCE_DIR}/lib/configs"
    )
endfunction()

add_library_test(load_lib_tests lib/load_library_tests.cpp)
add_library_test(bnn_tnn_tests lib/bnn_tnn_tests.cpp)
add_library_test(int_tests lib/int_tests.cpp)
