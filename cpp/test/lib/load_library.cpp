#include <gtest/gtest.h>
#include <dlfcn.h>

TEST(LibraryTests, LoadTest) {
    const char* ldLibraryPath = std::getenv("LD_LIBRARY_PATH");
    
    if (ldLibraryPath) {
        std::cout << "LD_LIBRARY_PATH: " << ldLibraryPath << std::endl;
    } else {
        std::cout << "LD_LIBRARY_PATH is not set." << std::endl;
    }

    void *handle = dlopen("libBNN_TNN_Interface.so", RTLD_LAZY);
    ASSERT_NE(handle, nullptr);
    dlclose(handle);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
