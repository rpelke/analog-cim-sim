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

#include "inc/test_helper.h"

namespace {

// The counters are printed when the crossbar is destroyed. Changing a key that
// forces a recreation destroys it while the test is still in control.
void run_accounting(const std::string &cfg_name) {
    std::string cfg = get_cfg_file(cfg_name);
    set_config(cfg.c_str());
    ASSERT_EQ(update_config(R"({"verbose": true})"), 0);

    int32_t mat[4] = {1, 1, -1, -1};
    ASSERT_EQ(cpy_mtrx(mat, 2, 2), 0);

    ASSERT_EQ(update_config(R"({"M": 8})"), 0);
}

} // namespace

// No mapping type should abort.
TEST(AccountingTests, VerboseOutputForEveryMappingType) {
    run_accounting("digital/I_DIFF_W_DIFF_1XB.json");
    run_accounting("digital/I_DIFF_W_DIFF_2XB.json");
    run_accounting("digital/I_OFFS_W_DIFF.json");
    run_accounting("digital/I_TC_W_DIFF.json");
    run_accounting("digital/I_UINT_W_DIFF.json");
    run_accounting("digital/I_UINT_W_OFFS.json");
    run_accounting("digital/BNN_I.json");
    run_accounting("digital/BNN_II.json");
    run_accounting("digital/BNN_III.json");
    run_accounting("digital/BNN_IV.json");
    run_accounting("digital/BNN_V.json");
    run_accounting("digital/BNN_VI.json");
    run_accounting("digital/TNN_I.json");
    run_accounting("digital/TNN_II.json");
    run_accounting("digital/TNN_III.json");
    run_accounting("digital/TNN_IV_split.json");
    run_accounting("digital/TNN_V_split.json");
}
