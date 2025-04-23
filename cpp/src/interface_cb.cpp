/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "stdio.h"
#include <iostream>

// Typedef for the function pointer for the C - Python conversion
typedef int32_t (*mvm_c_to_py_funcptr)(int32_t *res, int32_t *vec, int32_t *mat,
                                       int32_t m_matrix, int32_t n_matrix);

// Function pointer for the Python callback function
mvm_c_to_py_funcptr mvm_c_to_py = nullptr;

// Set funtion pointer
extern "C" void set_mvm_python_cb(mvm_c_to_py_funcptr cb) { mvm_c_to_py = cb; }

// Typedef for the function pointer for the C - Python conversion
typedef int32_t (*cpy_c_to_py_funcptr)(int32_t *mat, int32_t m_matrix,
                                       int32_t n_matrix, const char *l_name);

// Function pointer for the Python callback function
cpy_c_to_py_funcptr cpy_c_to_py = nullptr;

// Set funtion pointer
extern "C" void set_cpy_python_cb(cpy_c_to_py_funcptr cb) { cpy_c_to_py = cb; }

extern "C" void update_config(const char *json_config) { return; }

extern "C" int32_t exe_mvm(int32_t *res, int32_t *vec, int32_t *mat,
                           int32_t m_matrix, int32_t n_matrix,
                           const char *l_name = "Unknown") {
#ifdef DEBUG_MODE
    std::cout << "Matrix-vector multiplication" << std::endl;
    std::cout << "Layer: " << l_name << std::endl;
// Find max and min values in the input vector
#include <cstdint>
    int32_t max_val = INT32_MIN;
    int32_t min_val = INT32_MAX;
    for (int i = 0; i < n_matrix; ++i) {
        int32_t val = vec[i];
        if (val > max_val)
            max_val = val;
        if (val < min_val)
            min_val = val;
    }
    std::cout << "Input vector dimensions: " << n_matrix << std::endl;
    std::cout << "Max value: " << max_val << ", Min value: " << min_val
              << std::endl;
#endif
    static int warning_done = 0;
    if (!mvm_c_to_py) {
        if (!warning_done) {
            std::cout << "Function pointer mvm_c_to_py not set. Emulate "
                         "operation instead."
                      << std::endl;
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
#ifdef DEBUG_MODE
// Find max and min values in the result vector
#include <cstdint>
    max_val = INT32_MIN;
    min_val = INT32_MAX;
    for (int i = 0; i < m_matrix; ++i) {
        int32_t val = res[i];
        if (val > max_val)
            max_val = val;
        if (val < min_val)
            min_val = val;
    }
    std::cout << "Result vector dimensions: " << m_matrix << std::endl;
    std::cout << "Max value: " << max_val << ", Min value: " << min_val
              << std::endl;
#endif
    return 0;
}

extern "C" int32_t cpy_mtrx(int32_t *mat, int32_t m_matrix, int32_t n_matrix,
                            const char *l_name = "Unknown") {
#ifdef DEBUG_MODE
    std::cout << "Matrix copy" << std::endl;
    std::cout << "Layer: " << l_name << std::endl;
// Find max and min values in the matrix
#include <cstdint>
    int32_t max_val = INT32_MIN;
    int32_t min_val = INT32_MAX;
    for (int i = 0; i < m_matrix; ++i) {
        for (int j = 0; j < n_matrix; ++j) {
            int32_t val = mat[i * n_matrix + j];
            if (val > max_val)
                max_val = val;
            if (val < min_val)
                min_val = val;
        }
    }
    std::cout << "Matrix dimensions: " << m_matrix << "x" << n_matrix
              << std::endl;
    std::cout << "Max value: " << max_val << ", Min value: " << min_val
              << std::endl;
#endif
    static int warning_done = 0;
    if (!cpy_c_to_py) {
        if (!warning_done) {
            std::cout << "Function pointer cpy_c_to_py not set. Emulate "
                         "operation instead."
                      << std::endl;
            warning_done = 1;
        }
        return 0;
    }
    (*cpy_c_to_py)(mat, m_matrix, n_matrix, l_name);
    return 0;
}
