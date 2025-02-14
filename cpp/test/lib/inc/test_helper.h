#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#include <dlfcn.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <gtest/gtest.h>

void set_cfg_file_env(const char* file_name) {
    const char* cfg_dir = std::getenv("CFG_DIR_TESTS");
    EXPECT_NE(cfg_dir, nullptr) << "CFG_DIR_TESTS is not set.";
    std::string cfg_file = std::string(cfg_dir) + "/" + file_name;
    setenv("CFG_FILE", cfg_file.c_str(), 1);
}

#endif
