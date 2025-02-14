/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include <stdio.h>
#include <iostream>
#include <vector>

#include "helper/config.h"

bool load_cfg = nq::Config::get_cfg().load_cfg();

nq::CrossbarMapping xbar(
    CFG.M,
    CFG.N,
    CFG.W_BIT,
    CFG.I_BIT,
    CFG.SPLIT,
    CFG.digital_only,
    CFG.m_mode,
    CFG.HRS,
    CFG.LRS,
    CFG.adc_type,
    CFG.min_adc_curr,
    CFG.max_adc_curr,
    CFG.alpha,
    CFG.resolution,
    CFG.verbose
);

// Typedef for the function pointer for the C - Python conversion
typedef int32_t(*mvm_c_to_py_funcptr)(
    int32_t *res,
    int32_t *vec,
    int32_t *mat,
    int32_t m_matrix,
    int32_t n_matrix);

// Function pointer for the Python callback function
mvm_c_to_py_funcptr mvm_c_to_py = nullptr;

// Set funtion pointer to Python callback function
extern "C"
void set_mvm_python_cb(mvm_c_to_py_funcptr cb) {
    mvm_c_to_py = cb;
}

// Typedef for the function pointer for the C - Python conversion
typedef int32_t(*cpy_c_to_py_funcptr)(
    int32_t *mat,
    int32_t m_matrix,
    int32_t n_matrix);

// Function pointer for the Python callback function
cpy_c_to_py_funcptr cpy_c_to_py = nullptr;

// Set funtion pointer to Python callback function
extern "C"
void set_cpy_python_cb(cpy_c_to_py_funcptr cb) {
    cpy_c_to_py = cb;
}


extern "C"
int32_t exe_mvm(int32_t *res, int32_t *vec, int32_t *mat, int32_t m_matrix, int32_t n_matrix) {
    if (mvm_c_to_py) {
        (*mvm_c_to_py)(res, vec, mat, m_matrix, n_matrix);
        return 0;
    }
    xbar.mvm(res, vec, mat, m_matrix, n_matrix);
    return 0;
}

extern "C"
int32_t cpy_mtrx(int32_t *mat, int32_t m_matrix, int32_t n_matrix) {
    if (cpy_c_to_py) {
        (*cpy_c_to_py)(mat, m_matrix, n_matrix);
        return 0;
    }
    xbar.write(mat, m_matrix, n_matrix);
    return 0;
}
