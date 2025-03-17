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
#include <filesystem>
#include <gtest/gtest.h>
#include <iostream>
#include <string>

std::string get_cfg_file(const char *file_name, bool digital) {
    const char *cfg_dir = std::getenv("CFG_DIR_TESTS");
    EXPECT_NE(cfg_dir, nullptr) << "CFG_DIR_TESTS is not set.";
    if (digital) {
        std::string cfg_file = std::string(cfg_dir) + "/digital/" + file_name;
        return cfg_file;
    } else {
        std::string cfg_file = std::string(cfg_dir) + "/analog/" + file_name;
        return cfg_file;
    }
}

std::string findLibraryWithPrefix(const std::string &prefix,
                                  const std::string &search_paths) {
    std::istringstream path_stream(search_paths);
    std::string directory;

    while (std::getline(path_stream, directory, ':')) {
        if (!std::filesystem::exists(directory) ||
            !std::filesystem::is_directory(directory)) {
            std::cerr << "Skipping non-existent or invalid directory: "
                      << directory << std::endl;
            continue;
        }
        for (const auto &entry :
             std::filesystem::directory_iterator(directory)) {
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

const std::string get_lib(const char *env_var, const char *prefix) {
    const char *ld_library_path = std::getenv(env_var);
    const std::string lib_path = findLibraryWithPrefix(prefix, ld_library_path);
    return lib_path;
}

#endif
