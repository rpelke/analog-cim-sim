add_subdirectory(googletest EXCLUDE_FROM_ALL)

add_executable(load_lib_test
    lib/load_library.cpp
)
get_target_property(ld_library_path BNN_TNN_Interface BINARY_DIR)
target_link_libraries(load_lib_test
    gtest
    gtest_main
    dl
)
enable_testing()
add_test(NAME load_lib_test COMMAND load_lib_test)

set_tests_properties(load_lib_test
    PROPERTIES ENVIRONMENT
    LD_LIBRARY_PATH=${ld_library_path}:$ENV{LD_LIBRARY_PATH}
)
