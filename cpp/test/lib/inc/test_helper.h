/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#include <cstdlib>
#include <dlfcn.h>
#include <gtest/gtest.h>
#include <iostream>
#include <string>

void set_cfg_file_env(const char *file_name) {
    const char *cfg_dir = std::getenv("CFG_DIR_TESTS");
    EXPECT_NE(cfg_dir, nullptr) << "CFG_DIR_TESTS is not set.";
    std::string cfg_file = std::string(cfg_dir) + "/" + file_name;
    setenv("CFG_FILE", cfg_file.c_str(), 1);
}

#endif
