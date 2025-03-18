#include <cstdlib>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "inc/test_helper.h"

TEST(VarTests, VarTest) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 2;
    int32_t mat[m_matrix * n_matrix] = {1, 1, -1, -1, 1, -1};

    std::string cfg = get_cfg_file("variability/variability.json");
    set_config(cfg.c_str());

    int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";

    const std::vector<std::vector<float>> &ia_p_vec = get_ia_p();
    const std::vector<std::vector<float>> &ia_m_vec = get_ia_m();

    for (int32_t m = 0; m < m_matrix; m++) {
        for (int32_t n = 0; n < n_matrix; n++) {
            int32_t elem = m * n_matrix + n;
            if (mat[elem] == 1) {
                ASSERT_TRUE(ia_p_vec[m][n] > ia_m_vec[m][n]);
            } else if (mat[elem] == -1) {
                ASSERT_TRUE(ia_p_vec[m][n] < ia_m_vec[m][n]);
            }
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
