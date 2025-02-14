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
    void *handle_lbnntnn = dlopen("libBNN_TNN_Interface.so", RTLD_LAZY);
    ASSERT_NE(handle_lbnntnn, nullptr);
    dlclose(handle_lbnntnn);

    set_cfg_file_env("config.json");
    void *handle_lint = dlopen("libINT_Interface.so", RTLD_LAZY);
    ASSERT_NE(handle_lint, nullptr);
    dlclose(handle_lint);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
