/******************************************************************************
 * Copyright (C) 2026 Joel Klein                                              *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This work is licensed under the terms described in the LICENSE file        *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/

#include "mapping/mapper.h"
#include <cstddef>
#include <cstdint>
#include <gtest/gtest.h>
#include <optional>
#include <vector>

// Every mode has a unique name, and the name can be used to get back the mode.
TEST(MappingRegistryTests, EveryModeHasAUniqueName) {
    const size_t num_modes = static_cast<size_t>(nq::MappingMode::NUM_MODES);
    for (size_t i = 0; i < num_modes; ++i) {
        const nq::MappingMode mode = static_cast<nq::MappingMode>(i);
        EXPECT_EQ(nq::Mapper::mode_from_name(nq::Mapper::name_from_mode(mode)),
                  mode);
    }
    EXPECT_EQ(nq::Mapper::mode_from_name("NOT_A_MAPPING"), std::nullopt);
    EXPECT_EQ(nq::Mapper::name_from_mode(nq::MappingMode::NUM_MODES),
              "Unknown mode");
}

// How many physical cells one logical weight occupies, per mapping mode.
TEST(MappingRegistryTests, FactorsPerMode) {
    struct Expected {
        nq::MappingMode mode;
        size_t split_size;
        uint32_t col;
        uint32_t row;
    };

    // The INT modes spread a weight over |SPLIT| segments, the others do not.
    const std::vector<Expected> expected = {
        {nq::MappingMode::I_DIFF_W_DIFF_1XB, 3, 6, 1},
        {nq::MappingMode::I_DIFF_W_DIFF_2XB, 3, 6, 1},
        {nq::MappingMode::I_OFFS_W_DIFF, 3, 6, 1},
        {nq::MappingMode::I_TC_W_DIFF, 3, 6, 1},
        {nq::MappingMode::I_UINT_W_DIFF, 3, 6, 1},
        {nq::MappingMode::I_UINT_W_OFFS, 3, 3, 1},
        {nq::MappingMode::BNN_I, 1, 2, 1},
        {nq::MappingMode::BNN_II, 1, 2, 1},
        {nq::MappingMode::BNN_III, 1, 1, 1},
        {nq::MappingMode::BNN_IV, 1, 1, 1},
        {nq::MappingMode::BNN_V, 1, 1, 2},
        {nq::MappingMode::BNN_VI, 1, 2, 2},
        {nq::MappingMode::TNN_I, 1, 2, 2},
        {nq::MappingMode::TNN_II, 1, 2, 1},
        {nq::MappingMode::TNN_III, 1, 2, 1},
        {nq::MappingMode::TNN_IV, 2, 2, 1},
        {nq::MappingMode::TNN_V, 2, 2, 1},
    };

    ASSERT_EQ(expected.size(), static_cast<size_t>(nq::MappingMode::NUM_MODES));

    for (const Expected &e : expected) {
        const nq::XbarFactors factors =
            nq::Mapper::properties(e.mode).xbar_factors(e.split_size);
        EXPECT_EQ(factors.col, e.col)
            << "column factor of " << nq::Mapper::name_from_mode(e.mode);
        EXPECT_EQ(factors.row, e.row)
            << "row factor of " << nq::Mapper::name_from_mode(e.mode);
    }
}
