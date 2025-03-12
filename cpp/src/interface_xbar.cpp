/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include <iostream>
#include <memory>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <stdio.h>
#include <vector>

#include "helper/config.h"

#ifndef EXPORT_API
#define EXPORT_API __attribute__((visibility("default")))
#endif

bool cfg_loaded = nq::Config::get_cfg().load_cfg("");
std::unique_ptr<nq::Crossbar> xbar =
    (cfg_loaded) ? std::make_unique<nq::Crossbar>() : nullptr;

extern "C" EXPORT_API void set_config(const char *cfg_file) {
    if (xbar != nullptr) {
        xbar = nullptr;
    }
    nq::Config::get_cfg().load_cfg(cfg_file);
    xbar = std::make_unique<nq::Crossbar>();
}

extern "C" EXPORT_API int32_t exe_mvm(int32_t *res, int32_t *vec, int32_t *mat,
                                      int32_t m_matrix, int32_t n_matrix) {
    if (xbar == nullptr) {
        std::cerr << "Error: Crossbar is not initialized. Please call "
                     "set_config() first."
                  << std::endl;
        return -1;
    }
    if (m_matrix > CFG.M || n_matrix > CFG.N) {
        std::cerr << "Error: Matrix dimensions exceed the crossbar size."
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

extern "C" EXPORT_API int32_t cpy_mtrx(int32_t *mat, int32_t m_matrix,
                                       int32_t n_matrix,
                                       const char *l_name = "Unkown") {
    if (xbar == nullptr) {
        std::cerr << "Error: Crossbar is not initialized. Please call "
                     "set_config() first."
                  << std::endl;
        return -1;
    }
    if (m_matrix > CFG.M || n_matrix > CFG.N) {
        std::cerr << "Error: Matrix dimensions exceed the crossbar size."
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
