#include "helper/functions.h"

/********************** Helper functions **********************/
const void check_pointer(const size_t *const size) {
    if (size == nullptr) {
        std::cerr << "size is nullptr." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

const void check_xbar() {
    if (xbar == nullptr) {
        std::cerr << "Config missing. xbar not initialized." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}
