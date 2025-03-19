/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include <cstdlib>
#include <dlfcn.h>
#include <filesystem>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

#include "inc/test_helper.h"

const bool digital[2] = {true, false};

std::string digital_to_foldername(const bool digital) {
    return digital ? "digital/" : "analog/";
}

TEST(INTLibTests, I_DIFF_W_DIFF_1XB) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec[n_matrix] = {-120, 55};
    int32_t vec2[n_matrix] = {0, 10};
    int32_t mat[m_matrix * n_matrix] = {100, -32, 1, 0, 12, 1};

    for (bool d : digital) {
        std::string cfg =
            get_cfg_file(digital_to_foldername(d) + "I_DIFF_W_DIFF_1XB.json");
        set_config(cfg.c_str());
        int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix write operation failed.";

        int32_t res[m_matrix] = {1, 1, -1};
        status = exe_mvm(res, vec, mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
        ASSERT_THAT(res, ::testing::ElementsAre(-13759, -119, -1386));

        int32_t res2[m_matrix] = {0, 0, 0};
        status = exe_mvm(res2, vec2, mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
        ASSERT_THAT(res2, ::testing::ElementsAre(-320, 0, 10));
    }
}

TEST(INTLibTests, I_DIFF_W_DIFF_2XB) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec[n_matrix] = {-120, 55};
    int32_t vec2[n_matrix] = {0, 10};
    int32_t mat[m_matrix * n_matrix] = {100, -32, 1, 0, 12, 1};

    for (bool d : digital) {
        std::string cfg =
            get_cfg_file(digital_to_foldername(d) + "I_DIFF_W_DIFF_2XB.json");
        set_config(cfg.c_str());
        int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix write operation failed.";

        int32_t res[m_matrix] = {1, 1, -1};
        status = exe_mvm(res, vec, mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
        ASSERT_THAT(res, ::testing::ElementsAre(-13759, -119, -1386));

        int32_t res2[m_matrix] = {0, 0, 0};
        status = exe_mvm(res2, vec2, mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
        ASSERT_THAT(res2, ::testing::ElementsAre(-320, 0, 10));
    }
}

TEST(INTLibTests, I_OFFS_W_DIFF) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec[n_matrix] = {-120, 55};
    int32_t vec2[n_matrix] = {0, 10};
    int32_t mat[m_matrix * n_matrix] = {100, -32, 1, 0, 12, 1};

    for (bool d : digital) {
        std::string cfg =
            get_cfg_file(digital_to_foldername(d) + "I_OFFS_W_DIFF.json");
        set_config(cfg.c_str());
        int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix write operation failed.";

        int32_t res[m_matrix] = {1, 1, -1};
        status = exe_mvm(res, vec, mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
        ASSERT_THAT(res, ::testing::ElementsAre(-13759, -119, -1386));

        int32_t res2[m_matrix] = {0, 0, 0};
        status = exe_mvm(res2, vec2, mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
        ASSERT_THAT(res2, ::testing::ElementsAre(-320, 0, 10));
    }
}

TEST(INTLibTests, I_TC_W_DIFF) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec[n_matrix] = {-120, 55};
    int32_t vec2[n_matrix] = {0, 10};
    int32_t mat[m_matrix * n_matrix] = {100, -32, 1, 0, 12, 1};

    for (bool d : digital) {
        std::string cfg =
            get_cfg_file(digital_to_foldername(d) + "I_TC_W_DIFF.json");
        set_config(cfg.c_str());
        int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix write operation failed.";

        int32_t res[m_matrix] = {1, 1, -1};
        status = exe_mvm(res, vec, mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
        ASSERT_THAT(res, ::testing::ElementsAre(-13759, -119, -1386));

        int32_t res2[m_matrix] = {0, 0, 0};
        status = exe_mvm(res2, vec2, mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
        ASSERT_THAT(res2, ::testing::ElementsAre(-320, 0, 10));
    }
}

TEST(INTLibTests, I_UINT_W_DIFF) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec1[n_matrix] = {120, 55};
    int32_t vec2[n_matrix] = {0, 10};
    int32_t mat[m_matrix * n_matrix] = {100, -32, 1, 0, -12, 1};

    for (bool d : digital) {
        std::string cfg =
            get_cfg_file(digital_to_foldername(d) + "I_UINT_W_DIFF.json");
        set_config(cfg.c_str());
        int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix write operation failed.";

        int32_t res1[m_matrix] = {1, 1, -1};
        status = exe_mvm(res1, vec1, mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
        ASSERT_THAT(res1, ::testing::ElementsAre(10241, 121, -1386));

        int32_t res2[m_matrix] = {0, 0, 0};
        status = exe_mvm(res2, vec2, mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
        ASSERT_THAT(res2, ::testing::ElementsAre(-320, 0, 10));
    }
}

TEST(INTLibTests, I_UINT_W_OFFS) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec1[n_matrix] = {120, 55};
    int32_t vec2[n_matrix] = {0, 10};
    int32_t mat[m_matrix * n_matrix] = {100, -32, 1, 0, -12, 1};

    for (bool d : digital) {
        std::string cfg =
            get_cfg_file(digital_to_foldername(d) + "I_UINT_W_OFFS.json");
        set_config(cfg.c_str());
        int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix write operation failed.";

        int32_t res1[m_matrix] = {1, 1, -1};
        status = exe_mvm(res1, vec1, mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
        ASSERT_THAT(res1, ::testing::ElementsAre(10241, 121, -1386));

        int32_t res2[m_matrix] = {0, 0, 0};
        status = exe_mvm(res2, vec2, mat, m_matrix, n_matrix);
        ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
        ASSERT_THAT(res2, ::testing::ElementsAre(-320, 0, 10));
    }
}

TEST(INTLibTests, BNN_I) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec1[n_matrix] = {1, 1};
    int32_t vec2[n_matrix] = {-1, 1};
    int32_t mat[m_matrix * n_matrix] = {1, 1, -1, -1, 1, -1};

    for (bool d : digital) {
        std::string cfg = get_cfg_file(digital_to_foldername(d) + "BNN_I.json");
        set_config(cfg.c_str());
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
}

TEST(INTLibTests, BNN_II) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec1[n_matrix] = {1, 1};
    int32_t vec2[n_matrix] = {-1, 1};
    int32_t mat[m_matrix * n_matrix] = {1, 1, -1, -1, 1, -1};

    for (bool d : digital) {
        std::string cfg =
            get_cfg_file(digital_to_foldername(d) + "BNN_II.json");
        set_config(cfg.c_str());
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
}

TEST(INTLibTests, BNN_III) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec1[n_matrix] = {1, 1};
    int32_t vec2[n_matrix] = {-1, 1};
    int32_t mat[m_matrix * n_matrix] = {1, 1, -1, -1, 1, -1};

    for (bool d : digital) {
        std::string cfg =
            get_cfg_file(digital_to_foldername(d) + "BNN_III.json");
        set_config(cfg.c_str());
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
}

TEST(INTLibTests, BNN_IV) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec1[n_matrix] = {1, 1};
    int32_t vec2[n_matrix] = {-1, 1};
    int32_t mat[m_matrix * n_matrix] = {1, 1, -1, -1, 1, -1};

    for (bool d : digital) {
        std::string cfg =
            get_cfg_file(digital_to_foldername(d) + "BNN_IV.json");
        set_config(cfg.c_str());
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
}

TEST(INTLibTests, BNN_V) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec1[n_matrix] = {1, 1};
    int32_t vec2[n_matrix] = {-1, 1};
    int32_t mat[m_matrix * n_matrix] = {1, 1, -1, -1, 1, -1};

    for (bool d : digital) {
        std::string cfg = get_cfg_file(digital_to_foldername(d) + "BNN_V.json");
        set_config(cfg.c_str());
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
}

TEST(INTLibTests, BNN_VI) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t vec1[n_matrix] = {1, 1};
    int32_t vec2[n_matrix] = {-1, 1};
    int32_t mat[m_matrix * n_matrix] = {1, 1, -1, -1, 1, -1};

    for (bool d : digital) {
        std::string cfg =
            get_cfg_file(digital_to_foldername(d) + "BNN_VI.json");
        set_config(cfg.c_str());
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
}

TEST(INTLibTests, TNN_I) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 3;
    int32_t vec1[n_matrix] = {1, -1, 0};
    int32_t vec2[n_matrix] = {1, 0, 1};
    int32_t mat[m_matrix * n_matrix] = {1, 1, 1, -1, -1, -1, 0, -1, 1};

    for (bool d : digital) {
        std::string cfg = get_cfg_file(digital_to_foldername(d) + "TNN_I.json");
        set_config(cfg.c_str());
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
}

TEST(INTLibTests, TNN_II) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 3;
    int32_t vec1[n_matrix] = {1, -1, 0};
    int32_t vec2[n_matrix] = {1, 0, 1};
    int32_t mat[m_matrix * n_matrix] = {1, 1, 1, -1, -1, -1, 0, -1, 1};

    for (bool d : digital) {
        std::string cfg =
            get_cfg_file(digital_to_foldername(d) + "TNN_II.json");
        set_config(cfg.c_str());
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
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
