/******************************************************************************
 * Copyright (C) 2026 Joel Klein                                              *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This work is licensed under the terms described in the LICENSE file        *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/

#include <cstdint>
#include <gtest/gtest.h>

#include "mapping/mapping_properties.h"

namespace {

// Testing properties: Only the fields the geometry depends on are given a
// meaning here.
constexpr nq::MappingProperties geometry(uint32_t col_mult, uint32_t row_mult,
                                         bool split_in_cols) {
    return {nq::MappingMode::NUM_MODES,
            "GEOMETRY_ONLY",
            nq::MappingType::INT,
            col_mult,
            row_mult,
            split_in_cols,
            nq::OutputEnc::SINGLE,
            1,
            1,
            false};
}

} // namespace

// The second matrix is what the extra cell of a pair holds, whether the two sit
// side by side or on top of each other.
TEST(MappingPropertiesTests, NegativeMatrixFollowsThePairedCell) {
    EXPECT_FALSE(geometry(1, 1, false).uses_negative_matrix());
    EXPECT_TRUE(geometry(2, 1, false).uses_negative_matrix());
    EXPECT_TRUE(geometry(1, 2, false).uses_negative_matrix());
    EXPECT_TRUE(geometry(2, 2, false).uses_negative_matrix());
}

// SPLIT only widens a weight if the mapping puts its segments in columns.
TEST(MappingPropertiesTests, SplitWidensOnlyColumnSplitMappings) {
    EXPECT_EQ(geometry(1, 1, true).split_columns_per_weight(3), 3u);
    EXPECT_EQ(geometry(1, 1, false).split_columns_per_weight(3), 1u);

    const nq::XbarFactors split = geometry(2, 1, true).xbar_factors(3);
    EXPECT_EQ(split.col, 6u);
    EXPECT_EQ(split.row, 1u);

    const nq::XbarFactors whole = geometry(2, 2, false).xbar_factors(3);
    EXPECT_EQ(whole.col, 2u);
    EXPECT_EQ(whole.row, 2u);
}

// The capacity rounds down, so a partly usable weight does not count.
TEST(MappingPropertiesTests, CapacityRoundsDown) {
    const nq::XbarCapacity diff = geometry(2, 1, true).xbar_capacity(3, 32, 32);
    EXPECT_EQ(diff.m, 5u);
    EXPECT_EQ(diff.n, 32u);

    const nq::XbarCapacity quad =
        geometry(2, 2, false).xbar_capacity(1, 32, 32);
    EXPECT_EQ(quad.m, 16u);
    EXPECT_EQ(quad.n, 16u);

    // A crossbar too small for a single weight has no capacity at all.
    const nq::XbarCapacity tiny = geometry(2, 1, false).xbar_capacity(1, 1, 1);
    EXPECT_EQ(tiny.m, 0u);
    EXPECT_EQ(tiny.n, 1u);
}
