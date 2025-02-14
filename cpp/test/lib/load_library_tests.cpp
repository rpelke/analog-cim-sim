#include <gtest/gtest.h>
#include <dlfcn.h>
#include <cstdlib>

TEST(LibraryTests, LoadTest) {
    const char* ldLibraryPath = std::getenv("LD_LIBRARY_PATH");
    
    if (ldLibraryPath) {
        std::cout << "LD_LIBRARY_PATH: " << ldLibraryPath << std::endl;
    } else {
        std::cout << "LD_LIBRARY_PATH is not set." << std::endl;
    }

    void *handle_lbnntnn = dlopen("libBNN_TNN_Interface.so", RTLD_LAZY);
    ASSERT_NE(handle_lbnntnn, nullptr);
    dlclose(handle_lbnntnn);

    ASSERT_EQ(setenv("CFG_FILE", "configs/config.json", 1), 0) << "Failed to set environment variable";

    void *handle_lint = dlopen("libINT_Interface.so", RTLD_LAZY);
    ASSERT_NE(handle_lint, nullptr);
    dlclose(handle_lint);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
