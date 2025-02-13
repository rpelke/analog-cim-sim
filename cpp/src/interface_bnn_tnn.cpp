#include "stdio.h"
#include <iostream>

// Typedef for the function pointer for the C - Python conversion
typedef int32_t(*mvm_c_to_py_funcptr)(
    int32_t *res,
    int32_t *vec,
    int32_t *mat,
    int32_t m_matrix,
    int32_t n_matrix);

// Function pointer for the Python callback function
mvm_c_to_py_funcptr mvm_c_to_py = nullptr;

// Set funtion pointer
extern "C"
void set_mvm_python_cb(mvm_c_to_py_funcptr cb) {
    mvm_c_to_py = cb;
}

// Typedef for the function pointer for the C - Python conversion
typedef int32_t(*cpy_c_to_py_funcptr)(
    int32_t *mat,
    int32_t m_matrix,
    int32_t n_matrix,
    const char * l_name);

// Function pointer for the Python callback function
cpy_c_to_py_funcptr cpy_c_to_py = nullptr;

// Set funtion pointer
extern "C"
void set_cpy_python_cb(cpy_c_to_py_funcptr cb) {
    cpy_c_to_py = cb;
}

extern "C"
int32_t exe_mvm(int32_t *res, int32_t *vec, int32_t *mat, int32_t m_matrix, int32_t n_matrix) {
    static int warning_done = 0;
    if (!mvm_c_to_py) {
        if (!warning_done) {
            std::cout << "Function pointer mvm_c_to_py not set. Emulate operation instead." << std::endl;
            warning_done = 1;
        }
        for (int m = 0; m < m_matrix; ++m) {
            for (int n = 0; n < n_matrix; ++n) {
                res[m] += mat[n_matrix * m + n] * vec[n];
            }
        }
        return 0;
    }
    (*mvm_c_to_py)(res, vec, mat, m_matrix, n_matrix);
    return 0;
}

extern "C"
int32_t cpy_mtrx(int32_t *mat, int32_t m_matrix, int32_t n_matrix, const char * l_name) {
    #ifdef DEBUG_MODE
        std::cout << "Layer: " << l_name << std::endl;
    #endif
    static int warning_done = 0;
    if (!cpy_c_to_py) {
        if (!warning_done) {
            std::cout << "Function pointer cpy_c_to_py not set. Emulate operation instead." << std::endl;
            warning_done = 1;
        }
        return 0;
    }
    (*cpy_c_to_py)(mat, m_matrix, n_matrix, l_name);
    return 0;
}
