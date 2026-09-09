/******************************************************************************
 * Copyright (C) 2026 Joel Klein                                              *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This work is licensed under the terms described in the LICENSE file        *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include <cstdlib>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "inc/test_helper.h"

namespace {

// Shrink the crossbar so a small matrix sits exactly at the capacity.
void set_xbar_size(uint32_t m, uint32_t n) {
    const std::string json =
        "{\"M\": " + std::to_string(m) + ", \"N\": " + std::to_string(n) + "}";
    ASSERT_EQ(update_config(json.c_str()), 0);
}

// A matrix at the capacity is accepted, one weight more is rejected.
void check_capacity(const std::string &cfg_file, int32_t fitting_m,
                    int32_t fitting_n, int32_t oversized_m,
                    int32_t oversized_n) {
    std::vector<int32_t> mat(oversized_m * oversized_n, 1);
    std::vector<int32_t> vec(oversized_n, 1);
    std::vector<int32_t> res(oversized_m, 0);

    EXPECT_EQ(cpy_mtrx(mat.data(), fitting_m, fitting_n), 0)
        << cfg_file << " rejected a matrix that fits.";
    EXPECT_EQ(exe_mvm(res.data(), vec.data(), mat.data(), fitting_m, fitting_n),
              0)
        << cfg_file << " rejected an MVM that fits.";

    EXPECT_EQ(cpy_mtrx(mat.data(), oversized_m, oversized_n), -1)
        << cfg_file << " accepted a matrix that does not fit.";
    EXPECT_EQ(
        exe_mvm(res.data(), vec.data(), mat.data(), oversized_m, oversized_n),
        -1)
        << cfg_file << " accepted an MVM that does not fit.";
}

} // namespace

// Two columns per weight and three split segments, so six columns in total.
TEST(SizeCheckTests, DiffWeightsSplitColumns) {
    std::string cfg = get_cfg_file("digital/I_DIFF_W_DIFF_1XB.json");
    set_config(cfg.c_str());
    set_xbar_size(12, 4);
    check_capacity(cfg, 2, 4, 3, 4);
}

// One column per split segment, so twice the capacity of the mapping above.
TEST(SizeCheckTests, OffsetWeightsSplitColumns) {
    std::string cfg = get_cfg_file("digital/I_UINT_W_OFFS.json");
    set_config(cfg.c_str());
    set_xbar_size(12, 4);
    check_capacity(cfg, 4, 4, 5, 4);
}

// The two polarities sit in adjacent rows, so the rows run out first.
TEST(SizeCheckTests, DiffWeightsDoubleRows) {
    std::string cfg = get_cfg_file("analog/BNN_V.json");
    set_config(cfg.c_str());
    set_xbar_size(4, 8);
    check_capacity(cfg, 4, 4, 4, 5);
}

// Two rows by two columns per weight, so both directions run out.
TEST(SizeCheckTests, QuadWeights) {
    std::string cfg = get_cfg_file("analog/BNN_VI.json");
    set_config(cfg.c_str());
    set_xbar_size(8, 8);
    check_capacity(cfg, 4, 4, 5, 4);
    check_capacity(cfg, 4, 4, 4, 5);
}

// The solver returns two columns per output here, so the result buffer has to
// hold twice the outputs at full capacity.
TEST(SizeCheckTests, WeightBitsInColumns) {
    std::string cfg = get_cfg_file("analog/TNN_IV_split.json");
    set_config(cfg.c_str());
    set_xbar_size(8, 8);
    check_capacity(cfg, 4, 8, 5, 8);
}

// A rejected write must not touch the crossbar.
TEST(SizeCheckTests, RejectedWriteLeavesCrossbarUntouched) {
    std::string cfg = get_cfg_file("digital/BNN_I.json");
    set_config(cfg.c_str());
    set_xbar_size(4, 4);

    int32_t mat[4] = {1, 1, -1, -1};
    ASSERT_EQ(cpy_mtrx(mat, 2, 2), 0);
    const std::vector<std::vector<int32_t>> written = get_gd_p();

    int32_t too_big[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    ASSERT_EQ(cpy_mtrx(too_big, 3, 3), -1);
    ASSERT_EQ(get_gd_p(), written);
}
