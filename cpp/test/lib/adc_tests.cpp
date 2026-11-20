#include <cstdlib>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <utility>
#include <vector>

#include "inc/test_helper.h"

TEST(ADCTests, SymADCTest_I_DIFF) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 5;
    int32_t mat[m_matrix * n_matrix] = {-128, -128, -128, -128, -128,
                                        127,  127,  127,  127,  127,
                                        -12,  88,   65,   0,    -99};
    int32_t vec[n_matrix] = {-1, -1, -1, -1, -1};
    int32_t res[m_matrix] = {0, 0, 0};

    std::string cfg = get_cfg_file("analog/SYM_ADC_1.json");
    set_config(cfg.c_str());

    int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";

    status = exe_mvm(res, vec, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res, ::testing::ElementsAre(640, -635, -42));
}

TEST(ADCTests, SymADCTest_I_OFFS) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 5;
    int32_t mat[m_matrix * n_matrix] = {-128, -128, -128, -128, -128,
                                        127,  127,  127,  127,  127,
                                        -12,  88,   65,   0,    -99};
    int32_t vec[n_matrix] = {1, 1, 1, 1, 1};
    int32_t res[m_matrix] = {0, 0, 0};

    std::string cfg = get_cfg_file("analog/SYM_ADC_2.json");
    set_config(cfg.c_str());

    int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";

    status = exe_mvm(res, vec, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res, ::testing::ElementsAre(-640, 635, 42));
}

TEST(ADCTests, PosADCTest) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 5;
    int32_t mat[m_matrix * n_matrix] = {
        127, 127, 127, 127, 127, -128, -128, -128, -128, -128, 0, 0, 0, 0, 0};
    int32_t vec[n_matrix] = {1, 1, 1, 1, 1};
    int32_t res[m_matrix] = {0, 0, 0};

    std::string cfg = get_cfg_file("analog/POS_ADC_1.json");
    set_config(cfg.c_str());

    int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";

    status = exe_mvm(res, vec, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res, ::testing::ElementsAre(635, -637, -1));
}

TEST(ADCTests, SymADCCalibrationTest) {
    const int32_t m_matrix = 3;
    const int32_t n_matrix = 5;
    int32_t mat[m_matrix * n_matrix] = {-4, 4, -3, 3, -1, 1, 2,  1,
                                        2,  1, 5,  6, 7,  8, -20};
    int32_t vec[n_matrix] = {-1, -1, -1, -1, -1};
    int32_t res[m_matrix] = {0, 0, 0};

    std::string cfg = get_cfg_file("analog/SYM_ADC_1.json");
    set_config(cfg.c_str());
    update_config(R"(
      {
         "adc_calib_mode": "CALIB",
         "adc_calib_dict": {"test_layer": [-1.0, 1.0]}
      }
    )");

    int32_t status = cpy_mtrx(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";

    status = exe_mvm(res, vec, mat, m_matrix, n_matrix, "test_layer");
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_THAT(res, ::testing::ElementsAre(1, -7, -6));
}

// Executes a 4x32 MVM with the slices [1, 3, 4] of analog/<m_mode>.json and
// returns the analog and the exact result. The inputs start at vec_min.
void exe_sliced_mvm(const std::string &m_mode, const std::string &adc_type,
                    int32_t resolution, int32_t vec_min,
                    std::vector<int32_t> &res, std::vector<int32_t> &exp) {
    const int32_t m_matrix = 4;
    const int32_t n_matrix = 32;
    std::vector<int32_t> mat(m_matrix * n_matrix);
    std::vector<int32_t> vec(n_matrix);
    for (int32_t i = 0; i < m_matrix * n_matrix; ++i) {
        mat[i] = (i * 37 + 11) % 256 - 128;
    }
    for (int32_t i = 0; i < n_matrix; ++i) {
        vec[i] = vec_min + (i * 53 + 7) % 255;
    }
    exp.assign(m_matrix, 0);
    for (int32_t m = 0; m < m_matrix; ++m) {
        for (int32_t n = 0; n < n_matrix; ++n) {
            exp[m] += mat[m * n_matrix + n] * vec[n];
        }
    }
    res.assign(m_matrix, 0);

    std::string cfg = get_cfg_file("analog/" + m_mode + ".json");
    set_config(cfg.c_str());
    std::string update = R"({"adc_type": ")" + adc_type +
                         R"(", "resolution": )" + std::to_string(resolution) +
                         "}";
    ASSERT_EQ(update_config(update.c_str()), 0);

    int32_t status = cpy_mtrx(mat.data(), m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";

    status = exe_mvm(res.data(), vec.data(), mat.data(), m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
}

// 32 rows and 4 bit cells need 2 * 32 * 15 = 960 steps to resolve one level
TEST(ADCTests, SymLevelADCTest) {
    std::vector<int32_t> res, exp;
    exe_sliced_mvm("I_DIFF_W_DIFF_1XB", "SYM_LEVEL_ADC", 10, -127, res, exp);
    EXPECT_EQ(res, exp);

    // Same resolution, but the codes are not on the cell levels
    exe_sliced_mvm("I_DIFF_W_DIFF_1XB", "SYM_RANGE_ADC", 10, -127, res, exp);
    EXPECT_NE(res, exp);
}

TEST(ADCTests, SymLevelADCMappingsTest) {
    // Unsigned inputs for I_UINT_W_DIFF, signed ones for the others
    const std::vector<std::pair<std::string, int32_t>> modes = {
        {"I_DIFF_W_DIFF_1XB", -127}, {"I_DIFF_W_DIFF_2XB", -127},
        {"I_OFFS_W_DIFF", -127},     {"I_TC_W_DIFF", -127},
        {"I_UINT_W_DIFF", 0}};
    for (const auto &[m_mode, vec_min] : modes) {
        std::vector<int32_t> res, exp;
        exe_sliced_mvm(m_mode, "SYM_LEVEL_ADC", 20, vec_min, res, exp);
        EXPECT_EQ(res, exp) << m_mode;
    }
}

TEST(ADCTests, SymLevelADCLowResolutionTest) {
    // 9 bits: one code spans two levels of the 4 bit slice
    std::vector<int32_t> res, exp;
    exe_sliced_mvm("I_DIFF_W_DIFF_1XB", "SYM_LEVEL_ADC", 9, -127, res, exp);
    EXPECT_NE(res, exp);
}

TEST(ADCTests, SymLevelADCInvalidMappingTest) {
    std::string cfg = get_cfg_file("analog/BNN_I.json");
    set_config(cfg.c_str());
    EXPECT_EXIT(update_config(R"({"adc_type": "SYM_LEVEL_ADC"})"),
                ::testing::ExitedWithCode(EXIT_FAILURE), "SYM_LEVEL_ADC");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
