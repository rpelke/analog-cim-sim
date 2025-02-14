#include <gtest/gtest.h>
#include <dlfcn.h>
#include <cstdlib>
#include "inc/test_helper.h"

TEST(INTLibTests, MVMTest) {
    set_cfg_file_env("config.json");
    
    void *handle = dlopen("libINT_Interface.so", RTLD_LAZY);
    ASSERT_NE(handle, nullptr);

    auto cpy_mtrx_ptr = (int32_t (*)(int32_t *, int32_t, int32_t)) dlsym(handle, "cpy_mtrx");
    ASSERT_NE(cpy_mtrx_ptr, nullptr) << "Failed to load function cpy_mtrx";
    int32_t mat[4] = {100, -32, 0, 12};
    int32_t m_matrix = 2;
    int32_t n_matrix = 2;
    int32_t status = cpy_mtrx_ptr(mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix write operation failed.";

    auto exe_mvm_ptr = (int32_t (*)(int32_t *, int32_t *, int32_t *, int32_t, int32_t)) dlsym(handle, "exe_mvm");
    ASSERT_NE(exe_mvm_ptr, nullptr) << "Failed to load function exe_mvm";
    int32_t vec[2] = {-120, 55};
    int32_t res[2] = {1, 1};
    status = exe_mvm_ptr(res, vec, mat, m_matrix, n_matrix);
    ASSERT_EQ(status, 0) << "Matrix-vector multiplication failed.";
    ASSERT_EQ(res[0], -13759);
    ASSERT_EQ(res[1], 661);

    dlclose(handle);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
