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
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

extern "C" {
int32_t exe_mvm(int32_t *res, int32_t *vec, int32_t *mat, int32_t m_matrix,
                int32_t n_matrix);
int32_t cpy_mtrx(int32_t *mat, int32_t m_matrix, int32_t n_matrix,
                 const char *l_name = "Unkown");
void set_config(const char *cfg_file);
}

const bool digital[2] = {false, true};

TEST(INTLibTests, I_DIFF_W_DIFF_1XB) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec[n_matrix] = {-120, 55};
    int32_t mat[m_matrix * m_matrix] = {100, -32, 1, 0, 12, 1};

    for (bool d : digital) {
        std::string cfg = get_cfg_file("I_DIFF_W_DIFF_1XB.json", d);
        set_config(cfg.c_str());
        int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix write operation failed.";
        int32_t res[m_matrix] = {1, 1, -1};
        status = exe_mvm(res, vec, mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
        ASSERT_THAT(res, ::testing::ElementsAre(-13759, -119, -1386));
    }
}

TEST(INTLibTests, I_DIFF_W_DIFF_2XB) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec[n_matrix] = {-120, 55};
    int32_t mat[m_matrix * m_matrix] = {100, -32, 1, 0, 12, 1};

    for (bool d : digital) {
        std::string cfg = get_cfg_file("I_DIFF_W_DIFF_2XB.json", d);
        set_config(cfg.c_str());
        int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix write operation failed.";
        int32_t res[m_matrix] = {1, 1, -1};
        status = exe_mvm(res, vec, mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
        ASSERT_THAT(res, ::testing::ElementsAre(-13759, -119, -1386));
    }
}

TEST(INTLibTests, I_OFFS_W_DIFF) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec[n_matrix] = {-120, 55};
    int32_t mat[m_matrix * m_matrix] = {100, -32, 1, 0, 12, 1};

    for (bool d : digital) {
        std::string cfg = get_cfg_file("I_OFFS_W_DIFF.json", d);
        set_config(cfg.c_str());
        int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix write operation failed.";
        int32_t res[m_matrix] = {1, 1, -1};
        status = exe_mvm(res, vec, mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
        ASSERT_THAT(res, ::testing::ElementsAre(-13759, -119, -1386));
    }
}

TEST(INTLibTests, I_TC_W_DIFF) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec[n_matrix] = {-120, 55};
    int32_t mat[m_matrix * m_matrix] = {100, -32, 1, 0, 12, 1};

    for (bool d : digital) {
        std::string cfg = get_cfg_file("I_TC_W_DIFF.json", d);
        set_config(cfg.c_str());
        int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix write operation failed.";
        int32_t res[m_matrix] = {1, 1, -1};
        status = exe_mvm(res, vec, mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
        ASSERT_THAT(res, ::testing::ElementsAre(-13759, -119, -1386));
    }
}

TEST(INTLibTests, I_UINT_W_DIFF) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec[n_matrix] = {120, 55};
    int32_t mat[m_matrix * m_matrix] = {100, -32, 1, 0, -12, 1};

    for (bool d : digital) {
        std::string cfg = get_cfg_file("I_UINT_W_DIFF.json", d);
        set_config(cfg.c_str());
        int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix write operation failed.";
        int32_t res[m_matrix] = {1, 1, -1};
        status = exe_mvm(res, vec, mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
        ASSERT_THAT(res, ::testing::ElementsAre(10241, 121, -1386));
    }
}

TEST(INTLibTests, I_UINT_W_OFFS) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec[n_matrix] = {120, 55};
    int32_t mat[m_matrix * m_matrix] = {100, -32, 1, 0, -12, 1};

    for (bool d : digital) {
        std::string cfg = get_cfg_file("I_UINT_W_OFFS.json", d);
        set_config(cfg.c_str());
        int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix write operation failed.";
        int32_t res[m_matrix] = {1, 1, -1};
        status = exe_mvm(res, vec, mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
        ASSERT_THAT(res, ::testing::ElementsAre(10241, 121, -1386));
    }
}

TEST(INTLibTests, BNN_I) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec[n_matrix] = {1, 1};
    int32_t mat[m_matrix * m_matrix] = {1, 1, -1, -1, 1, -1};

    for (bool d : digital) {
        std::string cfg = get_cfg_file("BNN_I.json", d);
        set_config(cfg.c_str());
        int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix write operation failed.";
        int32_t res[m_matrix] = {1, -1, 1};
        status = exe_mvm(res, vec, mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
        ASSERT_THAT(res, ::testing::ElementsAre(3, -3, 1));
    }
}

TEST(INTLibTests, BNN_II) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec[n_matrix] = {1, 1};
    int32_t mat[m_matrix * m_matrix] = {1, 1, -1, -1, 1, -1};

    for (bool d : digital) {
        std::string cfg = get_cfg_file("BNN_II.json", d);
        set_config(cfg.c_str());
        int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix write operation failed.";
        int32_t res[m_matrix] = {1, -1, 1};
        status = exe_mvm(res, vec, mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
        ASSERT_THAT(res, ::testing::ElementsAre(3, -3, 1));
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
