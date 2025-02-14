#include <gtest/gtest.h>
#include <dlfcn.h>
#include <cstdlib>
#include "inc/test_helper.h"

TEST(LibraryTests, LoadTest) {
    void *handle_lbnntnn = dlopen("libBNN_TNN_Interface.so", RTLD_LAZY);
    ASSERT_NE(handle_lbnntnn, nullptr);
    dlclose(handle_lbnntnn);

    set_cfg_file_env("config.json");
    void *handle_lint = dlopen("libINT_Interface.so", RTLD_LAZY);
    ASSERT_NE(handle_lint, nullptr);
    dlclose(handle_lint);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
