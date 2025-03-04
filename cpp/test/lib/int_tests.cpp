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

TEST(INTLibTests, I_DIFF_W_DIFF_1XB) {
    set_cfg_file_env("I_DIFF_W_DIFF_1XB.json");
    std::string lib_path = get_lib("LD_LIBRARY_PATH", "acs_int");
    ASSERT_FALSE(lib_path.empty())
        << "No library found with prefix 'acs_int' in LD_LIBRARY_PATH";
    void *handle = dlopen(lib_path.c_str(), RTLD_LAZY);
    ASSERT_NE(handle, nullptr);

    auto cpy_mtrx_ptr =
        (int32_t(*)(int32_t *, int32_t, int32_t))dlsym(handle, "cpy_mtrx");
    ASSERT_NE(cpy_mtrx_ptr, nullptr) << "Failed to load function cpy_mtrx";
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t mat[m_matrix * m_matrix] = {100, -32, 1, 0, 12, 1};
    int32_t status = cpy_mtrx_ptr(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";
    auto exe_mvm_ptr = (int32_t(*)(int32_t *, int32_t *, int32_t *, int32_t,
                                   int32_t))dlsym(handle, "exe_mvm");
    ASSERT_NE(exe_mvm_ptr, nullptr) << "Failed to load function exe_mvm";
    int32_t vec[n_matrix] = {-120, 55};
    int32_t res[m_matrix] = {1, 1, -1};
    status = exe_mvm_ptr(res, vec, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_EQ(res[0], -13759);
    ASSERT_EQ(res[1], -119);
    ASSERT_EQ(res[2], -1386);
    dlclose(handle);
}

TEST(INTLibTests, I_DIFF_W_DIFF_2XB) {
    set_cfg_file_env("I_DIFF_W_DIFF_2XB.json");
    std::string lib_path = get_lib("LD_LIBRARY_PATH", "acs_int");
    ASSERT_FALSE(lib_path.empty())
        << "No library found with prefix 'acs_int' in LD_LIBRARY_PATH";
    void *handle = dlopen(lib_path.c_str(), RTLD_LAZY);
    ASSERT_NE(handle, nullptr);

    auto cpy_mtrx_ptr =
        (int32_t(*)(int32_t *, int32_t, int32_t))dlsym(handle, "cpy_mtrx");
    ASSERT_NE(cpy_mtrx_ptr, nullptr) << "Failed to load function cpy_mtrx";
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t mat[m_matrix * m_matrix] = {100, -32, 1, 0, 12, 1};
    int32_t status = cpy_mtrx_ptr(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";
    auto exe_mvm_ptr = (int32_t(*)(int32_t *, int32_t *, int32_t *, int32_t,
                                   int32_t))dlsym(handle, "exe_mvm");
    ASSERT_NE(exe_mvm_ptr, nullptr) << "Failed to load function exe_mvm";
    int32_t vec[n_matrix] = {-120, 55};
    int32_t res[m_matrix] = {1, 1, -1};
    status = exe_mvm_ptr(res, vec, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_EQ(res[0], -13759);
    ASSERT_EQ(res[1], -119);
    ASSERT_EQ(res[2], -1386);
    dlclose(handle);
}

TEST(INTLibTests, I_OFFS_W_DIFF) {
    set_cfg_file_env("I_OFFS_W_DIFF.json");
    std::string lib_path = get_lib("LD_LIBRARY_PATH", "acs_int");
    ASSERT_FALSE(lib_path.empty())
        << "No library found with prefix 'acs_int' in LD_LIBRARY_PATH";
    void *handle = dlopen(lib_path.c_str(), RTLD_LAZY);
    ASSERT_NE(handle, nullptr);

    auto cpy_mtrx_ptr =
        (int32_t(*)(int32_t *, int32_t, int32_t))dlsym(handle, "cpy_mtrx");
    ASSERT_NE(cpy_mtrx_ptr, nullptr) << "Failed to load function cpy_mtrx";
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t mat[m_matrix * m_matrix] = {100, -32, 1, 0, 12, 1};
    int32_t status = cpy_mtrx_ptr(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";
    auto exe_mvm_ptr = (int32_t(*)(int32_t *, int32_t *, int32_t *, int32_t,
                                   int32_t))dlsym(handle, "exe_mvm");
    ASSERT_NE(exe_mvm_ptr, nullptr) << "Failed to load function exe_mvm";
    int32_t vec[n_matrix] = {-120, 55};
    int32_t res[m_matrix] = {1, 1, -1};
    status = exe_mvm_ptr(res, vec, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_EQ(res[0], -13759);
    ASSERT_EQ(res[1], -119);
    ASSERT_EQ(res[2], -1386);
    dlclose(handle);
}

TEST(INTLibTests, I_TC_W_DIFF) {
    set_cfg_file_env("I_TC_W_DIFF.json");
    std::string lib_path = get_lib("LD_LIBRARY_PATH", "acs_int");
    ASSERT_FALSE(lib_path.empty())
        << "No library found with prefix 'acs_int' in LD_LIBRARY_PATH";
    void *handle = dlopen(lib_path.c_str(), RTLD_LAZY);
    ASSERT_NE(handle, nullptr);

    auto cpy_mtrx_ptr =
        (int32_t(*)(int32_t *, int32_t, int32_t))dlsym(handle, "cpy_mtrx");
    ASSERT_NE(cpy_mtrx_ptr, nullptr) << "Failed to load function cpy_mtrx";
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t mat[m_matrix * m_matrix] = {100, -32, 1, 0, 12, 1};
    int32_t status = cpy_mtrx_ptr(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";
    auto exe_mvm_ptr = (int32_t(*)(int32_t *, int32_t *, int32_t *, int32_t,
                                   int32_t))dlsym(handle, "exe_mvm");
    ASSERT_NE(exe_mvm_ptr, nullptr) << "Failed to load function exe_mvm";
    int32_t vec[n_matrix] = {-120, 55};
    int32_t res[m_matrix] = {1, 1, -1};
    status = exe_mvm_ptr(res, vec, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_EQ(res[0], -13759);
    ASSERT_EQ(res[1], -119);
    ASSERT_EQ(res[2], -1386);
    dlclose(handle);
}

TEST(INTLibTests, I_UINT_W_DIFF) {
    set_cfg_file_env("I_UINT_W_DIFF.json");
    std::string lib_path = get_lib("LD_LIBRARY_PATH", "acs_int");
    ASSERT_FALSE(lib_path.empty())
        << "No library found with prefix 'acs_int' in LD_LIBRARY_PATH";
    void *handle = dlopen(lib_path.c_str(), RTLD_LAZY);
    ASSERT_NE(handle, nullptr);

    auto cpy_mtrx_ptr =
        (int32_t(*)(int32_t *, int32_t, int32_t))dlsym(handle, "cpy_mtrx");
    ASSERT_NE(cpy_mtrx_ptr, nullptr) << "Failed to load function cpy_mtrx";
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t mat[m_matrix * m_matrix] = {100, -32, 1, 0, -12, 1};
    int32_t status = cpy_mtrx_ptr(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";
    auto exe_mvm_ptr = (int32_t(*)(int32_t *, int32_t *, int32_t *, int32_t,
                                   int32_t))dlsym(handle, "exe_mvm");
    ASSERT_NE(exe_mvm_ptr, nullptr) << "Failed to load function exe_mvm";
    int32_t vec[n_matrix] = {120, 55};
    int32_t res[m_matrix] = {1, 1, -1};
    status = exe_mvm_ptr(res, vec, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_EQ(res[0], 10241);
    ASSERT_EQ(res[1], 121);
    ASSERT_EQ(res[2], -1386);
    dlclose(handle);
}

TEST(INTLibTests, I_UINT_W_OFFS) {
    set_cfg_file_env("I_UINT_W_OFFS.json");
    std::string lib_path = get_lib("LD_LIBRARY_PATH", "acs_int");
    ASSERT_FALSE(lib_path.empty())
        << "No library found with prefix 'acs_int' in LD_LIBRARY_PATH";
    void *handle = dlopen(lib_path.c_str(), RTLD_LAZY);
    ASSERT_NE(handle, nullptr);

    auto cpy_mtrx_ptr =
        (int32_t(*)(int32_t *, int32_t, int32_t))dlsym(handle, "cpy_mtrx");
    ASSERT_NE(cpy_mtrx_ptr, nullptr) << "Failed to load function cpy_mtrx";
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t mat[m_matrix * m_matrix] = {100, -32, 1, 0, -12, 1};
    int32_t status = cpy_mtrx_ptr(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";
    auto exe_mvm_ptr = (int32_t(*)(int32_t *, int32_t *, int32_t *, int32_t,
                                   int32_t))dlsym(handle, "exe_mvm");
    ASSERT_NE(exe_mvm_ptr, nullptr) << "Failed to load function exe_mvm";
    int32_t vec[n_matrix] = {120, 55};
    int32_t res[m_matrix] = {1, 1, -1};
    status = exe_mvm_ptr(res, vec, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_EQ(res[0], 10241);
    ASSERT_EQ(res[1], 121);
    ASSERT_EQ(res[2], -1386);
    dlclose(handle);
}

TEST(INTLibTests, BNN_I) {
    set_cfg_file_env("BNN_I.json");
    std::string lib_path = get_lib("LD_LIBRARY_PATH", "acs_int");
    ASSERT_FALSE(lib_path.empty())
        << "No library found with prefix 'acs_int' in LD_LIBRARY_PATH";
    void *handle = dlopen(lib_path.c_str(), RTLD_LAZY);
    ASSERT_NE(handle, nullptr);

    auto cpy_mtrx_ptr =
        (int32_t(*)(int32_t *, int32_t, int32_t))dlsym(handle, "cpy_mtrx");
    ASSERT_NE(cpy_mtrx_ptr, nullptr) << "Failed to load function cpy_mtrx";
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t mat[m_matrix * m_matrix] = {1, 1, -1, -1, 1, -1};
    int32_t status = cpy_mtrx_ptr(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";
    auto exe_mvm_ptr = (int32_t(*)(int32_t *, int32_t *, int32_t *, int32_t,
                                   int32_t))dlsym(handle, "exe_mvm");
    ASSERT_NE(exe_mvm_ptr, nullptr) << "Failed to load function exe_mvm";
    int32_t vec[n_matrix] = {1, 1};
    int32_t res[m_matrix] = {0, 0, 0};
    status = exe_mvm_ptr(res, vec, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_EQ(res[0], 2);
    ASSERT_EQ(res[1], -2);
    ASSERT_EQ(res[2], 0);
    dlclose(handle);
}

TEST(INTLibTests, BNN_II) {
    set_cfg_file_env("BNN_II.json");
    std::string lib_path = get_lib("LD_LIBRARY_PATH", "acs_int");
    ASSERT_FALSE(lib_path.empty())
        << "No library found with prefix 'acs_int' in LD_LIBRARY_PATH";
    void *handle = dlopen(lib_path.c_str(), RTLD_LAZY);
    ASSERT_NE(handle, nullptr);

    auto cpy_mtrx_ptr =
        (int32_t(*)(int32_t *, int32_t, int32_t))dlsym(handle, "cpy_mtrx");
    ASSERT_NE(cpy_mtrx_ptr, nullptr) << "Failed to load function cpy_mtrx";
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t mat[m_matrix * m_matrix] = {1, 1, -1, -1, 1, -1};
    int32_t status = cpy_mtrx_ptr(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";
    auto exe_mvm_ptr = (int32_t(*)(int32_t *, int32_t *, int32_t *, int32_t,
                                   int32_t))dlsym(handle, "exe_mvm");
    ASSERT_NE(exe_mvm_ptr, nullptr) << "Failed to load function exe_mvm";
    int32_t vec[n_matrix] = {1, 1};
    int32_t res[m_matrix] = {0, 0, 0};
    status = exe_mvm_ptr(res, vec, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_EQ(res[0], 2);
    ASSERT_EQ(res[1], -2);
    ASSERT_EQ(res[2], 0);
    dlclose(handle);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
