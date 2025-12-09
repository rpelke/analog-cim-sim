/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan                   *
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

// C interface of acs_int
extern "C" {
int32_t exe_mvm(int32_t *res, int32_t *vec, int32_t *mat, int32_t m_matrix,
                int32_t n_matrix, const char *l_name = "Unknown");
int32_t cpy_mtrx(int32_t *mat, int32_t m_matrix, int32_t n_matrix,
                 const char *l_name = "Unkown");
void set_config(const char *cfg_file);
int32_t update_config(const char *json_config, const char *l_name = "Unknown");
const void *get_ia_p(size_t *size);
const void *get_ia_m(size_t *size);
const void *get_gd_p(size_t *size);
const void *get_gd_m(size_t *size);
}

// C++ interface of acs_int
extern const std::vector<std::vector<float>> &get_ia_p();
extern const std::vector<std::vector<float>> &get_ia_m();
extern const std::vector<std::vector<int32_t>> &get_gd_p();
extern const std::vector<std::vector<int32_t>> &get_gd_m();
extern const std::string get_adc_profile();

std::string get_cfg_file(const std::string &file_name) {
    const char *cfg_dir = std::getenv("CFG_DIR_TESTS");
    EXPECT_NE(cfg_dir, nullptr) << "CFG_DIR_TESTS is not set.";
    std::string cfg_file = std::string(cfg_dir) + "/" + file_name;
    return cfg_file;
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
