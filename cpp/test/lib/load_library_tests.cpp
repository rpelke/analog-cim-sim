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
#include <filesystem>
#include <gtest/gtest.h>
#include <iostream>

namespace fs = std::filesystem;

std::string findLibraryWithPrefix(const std::string &prefix,
                                  const std::string &search_paths) {
    std::istringstream path_stream(search_paths);
    std::string directory;

    while (std::getline(path_stream, directory, ':')) {
        if (!fs::exists(directory) || !fs::is_directory(directory)) {
            std::cerr << "Skipping non-existent or invalid directory: "
                      << directory << std::endl;
            continue;
        }
        for (const auto &entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::cerr << "Checking file: " << filename << std::endl;
                if (filename.rfind(prefix, 0) == 0) {
                    return entry.path().string();
                }
            }
        }
    }
    return "";
}

TEST(LibraryTests, LoadTest) {
    void *handle_lbnntnn = dlopen("libacs_bnntnn.so", RTLD_LAZY);
    ASSERT_NE(handle_lbnntnn, nullptr);
    dlclose(handle_lbnntnn);

    const char *ld_library_path = std::getenv("LD_LIBRARY_PATH");
    ASSERT_NE(ld_library_path, nullptr) << "LD_LIBRARY_PATH is not set";
    std::string lib_path = findLibraryWithPrefix("acs_int", ld_library_path);
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
