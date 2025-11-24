/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan                   *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include <cstdlib>
#include <dlfcn.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

#include "inc/test_helper.h"

TEST(ParasiticsTests, BNN_I) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec1[n_matrix] = {1, 1};
    int32_t vec2[n_matrix] = {-1, 1};
    int32_t mat[m_matrix * n_matrix] = {1, 1, -1, -1, 1, -1};

    std::string cfg = get_cfg_file("analog/BNN_I.json");
    set_config(cfg.c_str());
    update_config(R"(
      {
         "parasitics": true
      }
    )");
    int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";

    int32_t res1[m_matrix] = {1, -1, 1};
    status = exe_mvm(res1, vec1, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res1, ::testing::ElementsAre(3, -3, 1));

    int32_t res2[m_matrix] = {0, 0, 0};
    status = exe_mvm(res2, vec2, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res2, ::testing::ElementsAre(0, 0, -2));
}

TEST(ParasiticsTests, BNN_II) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec1[n_matrix] = {1, 1};
    int32_t vec2[n_matrix] = {-1, 1};
    int32_t mat[m_matrix * n_matrix] = {1, 1, -1, -1, 1, -1};

    std::string cfg = get_cfg_file("analog/BNN_II.json");
    set_config(cfg.c_str());
    update_config(R"(
      {
         "parasitics": true
      }
    )");
    int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";

    int32_t res1[m_matrix] = {1, -1, 1};
    status = exe_mvm(res1, vec1, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res1, ::testing::ElementsAre(3, -3, 1));

    int32_t res2[m_matrix] = {0, 0, 0};
    status = exe_mvm(res2, vec2, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res2, ::testing::ElementsAre(0, 0, -2));
}

TEST(ParasiticsTests, BNN_III) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec1[n_matrix] = {1, 1};
    int32_t vec2[n_matrix] = {-1, 1};
    int32_t mat[m_matrix * n_matrix] = {1, 1, -1, -1, 1, -1};

    std::string cfg = get_cfg_file("analog/BNN_III.json");
    set_config(cfg.c_str());
    update_config(R"(
      {
         "parasitics": true
      }
    )");
    int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";

    int32_t res1[m_matrix] = {1, -1, 1};
    status = exe_mvm(res1, vec1, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res1, ::testing::ElementsAre(3, -3, 1));

    int32_t res2[m_matrix] = {0, 0, 0};
    status = exe_mvm(res2, vec2, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res2, ::testing::ElementsAre(0, 0, -2));
}

TEST(ParasiticsTests, BNN_IV) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec1[n_matrix] = {1, 1};
    int32_t vec2[n_matrix] = {-1, 1};
    int32_t mat[m_matrix * n_matrix] = {1, 1, -1, -1, 1, -1};

    std::string cfg = get_cfg_file("analog/BNN_IV.json");
    set_config(cfg.c_str());
    update_config(R"(
      {
         "parasitics": true
      }
    )");
    int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";

    int32_t res1[m_matrix] = {1, -1, 1};
    status = exe_mvm(res1, vec1, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res1, ::testing::ElementsAre(3, -3, 1));

    int32_t res2[m_matrix] = {0, 0, 0};
    status = exe_mvm(res2, vec2, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res2, ::testing::ElementsAre(0, 0, -2));
}

TEST(ParasiticsTests, BNN_V) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec1[n_matrix] = {1, 1};
    int32_t vec2[n_matrix] = {-1, 1};
    int32_t mat[m_matrix * n_matrix] = {1, 1, -1, -1, 1, -1};

    std::string cfg = get_cfg_file("analog/BNN_V.json");
    set_config(cfg.c_str());
    update_config(R"(
      {
         "parasitics": true
      }
    )");
    int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";

    int32_t res1[m_matrix] = {1, -1, 1};
    status = exe_mvm(res1, vec1, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res1, ::testing::ElementsAre(3, -3, 1));

    int32_t res2[m_matrix] = {0, 0, 0};
    status = exe_mvm(res2, vec2, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res2, ::testing::ElementsAre(0, 0, -2));
}

TEST(ParasiticsTests, BNN_VI) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec1[n_matrix] = {1, 1};
    int32_t vec2[n_matrix] = {-1, 1};
    int32_t mat[m_matrix * n_matrix] = {1, 1, -1, -1, 1, -1};

    std::string cfg = get_cfg_file("analog/BNN_VI.json");
    set_config(cfg.c_str());
    update_config(R"(
      {
         "parasitics": true
      }
    )");
    int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";

    int32_t res1[m_matrix] = {1, -1, 1};
    status = exe_mvm(res1, vec1, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res1, ::testing::ElementsAre(3, -3, 1));

    int32_t res2[m_matrix] = {0, 0, 0};
    status = exe_mvm(res2, vec2, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res2, ::testing::ElementsAre(0, 0, -2));
}

TEST(ParasiticsTests, TNN_I) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 3;
    int32_t vec1[n_matrix] = {1, -1, 0};
    int32_t vec2[n_matrix] = {1, 0, 1};
    int32_t mat[m_matrix * n_matrix] = {1, 1, 1, -1, -1, -1, 0, -1, 1};

    std::string cfg = get_cfg_file("analog/TNN_I.json");
    set_config(cfg.c_str());
    update_config(R"(
      {
         "parasitics": true
      }
    )");
    int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";

    int32_t res1[m_matrix] = {1, -1, 1};
    status = exe_mvm(res1, vec1, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res1, ::testing::ElementsAre(1, -1, 2));

    int32_t res2[m_matrix] = {0, 0, 0};
    status = exe_mvm(res2, vec2, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res2, ::testing::ElementsAre(2, -2, 1));
}

TEST(ParasiticsTests, TNN_II) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 3;
    int32_t vec1[n_matrix] = {1, -1, 0};
    int32_t vec2[n_matrix] = {1, 0, 1};
    int32_t mat[m_matrix * n_matrix] = {1, 1, 1, -1, -1, -1, 0, -1, 1};

    std::string cfg = get_cfg_file("analog/TNN_II.json");
    set_config(cfg.c_str());
    update_config(R"(
      {
         "parasitics": true
      }
    )");
    int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";

    int32_t res1[m_matrix] = {1, -1, 1};
    status = exe_mvm(res1, vec1, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res1, ::testing::ElementsAre(1, -1, 2));

    int32_t res2[m_matrix] = {0, 0, 0};
    status = exe_mvm(res2, vec2, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res2, ::testing::ElementsAre(2, -2, 1));
}

TEST(ParasiticsTests, TNN_III) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 3;
    int32_t vec1[n_matrix] = {1, -1, 0};
    int32_t vec2[n_matrix] = {1, 0, 1};
    int32_t mat[m_matrix * n_matrix] = {1, 1, 1, -1, -1, -1, 0, -1, 1};

    std::string cfg = get_cfg_file("analog/TNN_III.json");
    set_config(cfg.c_str());
    update_config(R"(
      {
         "parasitics": true
      }
    )");
    int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";

    int32_t res1[m_matrix] = {1, -1, 1};
    status = exe_mvm(res1, vec1, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res1, ::testing::ElementsAre(1, -1, 2));

    int32_t res2[m_matrix] = {0, 0, 0};
    status = exe_mvm(res2, vec2, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res2, ::testing::ElementsAre(2, -2, 1));
}

TEST(ParasiticsTests, TNN_IV_split) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 3;
    int32_t vec1[n_matrix] = {1, -1, 0};
    int32_t vec2[n_matrix] = {1, 0, 1};
    int32_t mat[m_matrix * n_matrix] = {1, 1, 1, -1, -1, -1, 0, -1, 1};

    std::string cfg = get_cfg_file("analog/TNN_IV_split.json");
    set_config(cfg.c_str());
    update_config(R"(
      {
         "parasitics": true
      }
    )");
    int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";

    int32_t res1[m_matrix] = {1, -1, 1};
    status = exe_mvm(res1, vec1, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res1, ::testing::ElementsAre(1, -1, 2));

    int32_t res2[m_matrix] = {0, 0, 0};
    status = exe_mvm(res2, vec2, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res2, ::testing::ElementsAre(2, -2, 1));
}

TEST(ParasiticsTests, TNN_V_split) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 3;
    int32_t vec1[n_matrix] = {1, -1, 0};
    int32_t vec2[n_matrix] = {1, 0, 1};
    int32_t mat[m_matrix * n_matrix] = {1, 1, 1, -1, -1, -1, 0, -1, 1};

    std::string cfg = get_cfg_file("analog/TNN_V_split.json");
    set_config(cfg.c_str());
    update_config(R"(
      {
         "parasitics": true
      }
    )");
    int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";

    int32_t res1[m_matrix] = {1, -1, 1};
    status = exe_mvm(res1, vec1, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res1, ::testing::ElementsAre(1, -1, 2));

    int32_t res2[m_matrix] = {0, 0, 0};
    status = exe_mvm(res2, vec2, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res2, ::testing::ElementsAre(2, -2, 1));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
