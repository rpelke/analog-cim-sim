/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include <cstdlib>
#include <dlfcn.h>
#include <gtest/gtest.h>

TEST(BNNTNNLibTests, MVMTest) {
    void *handle = dlopen("libacs_bnntnn.so", RTLD_LAZY);
    ASSERT_NE(handle, nullptr);

    auto exe_mvm_ptr = (int32_t(*)(int32_t *, int32_t *, int32_t *, int32_t,
                                   int32_t))dlsym(handle, "exe_mvm");
    ASSERT_NE(exe_mvm_ptr, nullptr) << "Failed to load function exe_mvm";

    int32_t res[2] = {0, 0};
    int32_t vec[2] = {1, -1};
    int32_t mat[4] = {1, 2, 3, -1};
    int32_t m_matrix = 2;
    int32_t n_matrix = 2;

    int32_t status = exe_mvm_ptr(res, vec, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed";

    EXPECT_EQ(res[0], -1);
    EXPECT_EQ(res[1], 4);

    dlclose(handle);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
