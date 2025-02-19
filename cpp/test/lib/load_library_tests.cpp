/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "inc/test_helper.h"
#include <cstdlib>
#include <dlfcn.h>
#include <gtest/gtest.h>

TEST(LibraryTests, LoadTest) {
    void *handle_lbnntnn = dlopen("libacs_bnntnn.so", RTLD_LAZY);
    ASSERT_NE(handle_lbnntnn, nullptr);
    dlclose(handle_lbnntnn);

    std::string lib_path = get_lib("LD_LIBRARY_PATH", "acs_int");
    ASSERT_FALSE(lib_path.empty())
        << "No library found with prefix 'acs_int' in LD_LIBRARY_PATH";
    void *handle_int = dlopen(lib_path.c_str(), RTLD_LAZY);
    ASSERT_NE(handle_int, nullptr);
    dlclose(handle_int);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
