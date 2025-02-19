/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include <iostream>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <stdio.h>
#include <vector>

#include "helper/config.h"

nq::CrossbarMapping *xbar = nullptr;

extern "C" void set_config(const char *cfg_file) {
    if (xbar != nullptr) {
        delete xbar;
        xbar = nullptr;
    }
    nq::Config::get_cfg().load_cfg(cfg_file);
    xbar = new nq::CrossbarMapping(
        CFG.M, CFG.N, CFG.W_BIT, CFG.I_BIT, CFG.SPLIT, CFG.digital_only,
        CFG.m_mode, CFG.HRS, CFG.LRS, CFG.adc_type, CFG.min_adc_curr,
        CFG.max_adc_curr, CFG.alpha, CFG.resolution, CFG.verbose);
}

extern "C" int32_t exe_mvm(int32_t *res, int32_t *vec, int32_t *mat,
                           int32_t m_matrix, int32_t n_matrix) {
    if (xbar == nullptr) {
        std::cerr << "Error: Crossbar is not initialized. Please call "
                     "set_config() first."
                  << std::endl;
        return -1;
    }
    xbar->mvm(res, vec, mat, m_matrix, n_matrix);
    return 0;
}

extern "C" int32_t exe_mvm_pb(pybind11::array_t<int32_t> res,
                              pybind11::array_t<int32_t> vec,
                              pybind11::array_t<int32_t> mat, int32_t m_matrix,
                              int32_t n_matrix) {
    auto res_buffer = res.request();
    auto vec_buffer = vec.request();
    auto mat_buffer = mat.request();

    int32_t *res_ptr = static_cast<int32_t *>(res_buffer.ptr);
    int32_t *vec_ptr = static_cast<int32_t *>(vec_buffer.ptr);
    int32_t *mat_ptr = static_cast<int32_t *>(mat_buffer.ptr);

    xbar->mvm(res_ptr, vec_ptr, mat_ptr, m_matrix, n_matrix);
    return 0;
}

extern "C" int32_t cpy_mtrx(int32_t *mat, int32_t m_matrix, int32_t n_matrix) {
    if (xbar == nullptr) {
        std::cerr << "Error: Crossbar is not initialized. Please call "
                     "set_config() first."
                  << std::endl;
        return -1;
    }
    xbar->write(mat, m_matrix, n_matrix);
    return 0;
}

extern "C" int32_t cpy_mtrx_pb(pybind11::array_t<int32_t> mat, int32_t m_matrix,
                               int32_t n_matrix) {
    auto mat_buffer = mat.request();
    int32_t *mat_ptr = static_cast<int32_t *>(mat_buffer.ptr);
    return cpy_mtrx(mat_ptr, m_matrix, n_matrix);
}

PYBIND11_MODULE(acs_int, m) {
    m.def("cpy", &cpy_mtrx_pb, "Copy matrix to crossbar.");
    m.def("mvm", &exe_mvm_pb, "Execute matrix-vector multiplication.");
    m.def("set_config", &set_config, "Set a config for the crossbar.");
}
