/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke & Joel Klein                              *
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

/********************** Global variables **********************/
bool cfg_loaded = nq::Config::get_cfg().load_cfg("");
std::unique_ptr<nq::Crossbar> xbar =
    (cfg_loaded) ? std::make_unique<nq::Crossbar>() : nullptr;

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

template <typename T>
const uint32_t num_matrix_elems(const std::vector<std::vector<T>> &mat) {
    uint32_t size = 0;
    for (const auto &inner_vector : mat) {
        size += inner_vector.size();
    }
    return size;
}

/************************ C interface ************************/
extern "C" EXPORT_API void set_config(const char *cfg_file) {
    xbar = nullptr;
    nq::Config::get_cfg().load_cfg(cfg_file);
    xbar = std::make_unique<nq::Crossbar>();
}

extern "C" EXPORT_API void update_config(const char *json_config) {
    if (json_config == nullptr) {
        std::cerr << "Warning: Config JSON is null. No changes applied."
                  << std::endl;
        return;
    }

    check_xbar();

    // Track whether parameters requiring crossbar recreation were updated
    bool recreate_xbar = false;

    // Let Config class handle the JSON parsing and updates
    bool config_updated =
        nq::Config::get_cfg().update_cfg(json_config, &recreate_xbar);

    // Only recreate crossbar if necessary keys were updated
    if (config_updated && recreate_xbar) {
        xbar = std::make_unique<nq::Crossbar>();
    }
}

extern "C" EXPORT_API int32_t exe_mvm(int32_t *res, int32_t *vec, int32_t *mat,
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

extern "C" EXPORT_API int32_t cpy_mtrx(int32_t *mat, int32_t m_matrix,
                                       int32_t n_matrix,
                                       const char *l_name = "Unkown") {
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

extern "C" EXPORT_API const void *get_gd_p(size_t *size) {
    check_pointer(size);
    check_xbar();
    const auto &gd_p = xbar->get_gd_p();
    if (gd_p.empty()) {
        *size = 0;
        return nullptr;
    }
    *size = num_matrix_elems(gd_p);
    const void *ptr = static_cast<const void *>(&gd_p);
    return ptr;
}

extern "C" EXPORT_API const void *get_gd_m(size_t *size) {
    check_pointer(size);
    check_xbar();
    const auto &gd_m = xbar->get_gd_m();
    if (gd_m.empty()) {
        *size = 0;
        return nullptr;
    }
    *size = num_matrix_elems(gd_m);
    const void *ptr = static_cast<const void *>(&gd_m);
    return ptr;
}

extern "C" EXPORT_API const void *get_ia_p(size_t *size) {
    check_pointer(size);
    check_xbar();
    const auto &ia_p = xbar->get_ia_p();
    if (ia_p.empty()) {
        *size = 0;
        return nullptr;
    }
    *size = num_matrix_elems(ia_p);
    const void *ptr = static_cast<const void *>(&ia_p);
    return ptr;
}

extern "C" EXPORT_API const void *get_ia_m(size_t *size) {
    check_pointer(size);
    check_xbar();
    const auto &ia_m = xbar->get_ia_m();
    if (ia_m.empty()) {
        *size = 0;
        return nullptr;
    }
    *size = num_matrix_elems(ia_m);
    const void *ptr = static_cast<const void *>(&ia_m);
    return ptr;
}

extern "C" EXPORT_API const uint64_t get_write_xbar_counter() {
    check_xbar();
    return xbar->get_write_xbar_counter();
}

extern "C" EXPORT_API const uint64_t get_mvm_counter() {
    check_xbar();
    return xbar->get_mvm_counter();
}

extern "C" EXPORT_API const uint64_t get_read_num() {
    check_xbar();
    return xbar->get_read_num();
}

/********************* Pybind interface *********************/
int32_t exe_mvm_pb(pybind11::array_t<int32_t> res,
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

int32_t cpy_mtrx_pb(pybind11::array_t<int32_t> mat, int32_t m_matrix,
                    int32_t n_matrix) {
    auto mat_buffer = mat.request();
    int32_t *mat_ptr = static_cast<int32_t *>(mat_buffer.ptr);
    return cpy_mtrx(mat_ptr, m_matrix, n_matrix);
}

pybind11::array_t<uint32_t> get_gd_p_pb() {
    check_xbar();

    const auto &gd_p = xbar->get_gd_p();
    size_t rows = gd_p.size();
    size_t cols = (rows > 0) ? gd_p[0].size() : 0;

    pybind11::array_t<uint32_t> result({rows, cols});
    auto r = result.mutable_unchecked<2>();
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            r(i, j) = gd_p[i][j];
        }
    }
    return result;
}

pybind11::array_t<uint32_t> get_gd_m_pb() {
    check_xbar();

    const auto &gd_m = xbar->get_gd_m();
    size_t rows = gd_m.size();
    size_t cols = (rows > 0) ? gd_m[0].size() : 0;

    pybind11::array_t<uint32_t> result({rows, cols});
    auto r = result.mutable_unchecked<2>();
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            r(i, j) = gd_m[i][j];
        }
    }
    return result;
}

pybind11::array_t<float> get_ia_p_pb() {
    check_xbar();

    const auto &ia_p = xbar->get_ia_p();
    size_t rows = ia_p.size();
    size_t cols = (rows > 0) ? ia_p[0].size() : 0;

    pybind11::array_t<float> result({rows, cols});
    auto r = result.mutable_unchecked<2>();
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            r(i, j) = ia_p[i][j];
        }
    }
    return result;
}

pybind11::array_t<float> get_ia_m_pb() {
    check_xbar();

    const auto &ia_m = xbar->get_ia_m();
    size_t rows = ia_m.size();
    size_t cols = (rows > 0) ? ia_m[0].size() : 0;

    pybind11::array_t<float> result({rows, cols});
    auto r = result.mutable_unchecked<2>();
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            r(i, j) = ia_m[i][j];
        }
    }
    return result;
}

pybind11::array_t<uint64_t> get_cycles_p_pb() {
    check_xbar();

    const auto &cycles_p = xbar->get_cycles_p();
    size_t rows = cycles_p.size();
    size_t cols = (rows > 0) ? cycles_p[0].size() : 0;

    pybind11::array_t<uint64_t> result({rows, cols});
    auto r = result.mutable_unchecked<2>();
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            r(i, j) = cycles_p[i][j];
        }
    }
    return result;
}

pybind11::array_t<uint64_t> get_cycles_m_pb() {
    check_xbar();

    const auto &cycles_m = xbar->get_cycles_m();
    size_t rows = cycles_m.size();
    size_t cols = (rows > 0) ? cycles_m[0].size() : 0;

    pybind11::array_t<uint64_t> result({rows, cols});
    auto r = result.mutable_unchecked<2>();
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            r(i, j) = cycles_m[i][j];
        }
    }
    return result;
}

void update_config_pb(const std::string &json_config) {
    // Check if JSON string is empty
    if (json_config.empty()) {
        std::cerr << "Warning: Config JSON is empty. No changes applied."
                  << std::endl;
        return;
    }

    // Call the C interface function
    update_config(json_config.c_str());
}

/*********************** C++ interface ***********************/
EXPORT_API const std::vector<std::vector<int32_t>> &get_gd_p() {
    return xbar->get_gd_p();
}

EXPORT_API const std::vector<std::vector<int32_t>> &get_gd_m() {
    return xbar->get_gd_m();
}

EXPORT_API const std::vector<std::vector<float>> &get_ia_p() {
    return xbar->get_ia_p();
}

EXPORT_API const std::vector<std::vector<float>> &get_ia_m() {
    return xbar->get_ia_m();
}

EXPORT_API const std::vector<std::vector<uint64_t>> &get_cycles_p() {
    return xbar->get_cycles_p();
}

EXPORT_API const std::vector<std::vector<uint64_t>> &get_cycles_m() {
    return xbar->get_cycles_m();
}

/********************* Pybind definitions *********************/
PYBIND11_MODULE(acs_int, m) {
    m.def("cpy", &cpy_mtrx_pb, "Copy matrix to crossbar.");
    m.def("mvm", &exe_mvm_pb, "Execute matrix-vector multiplication.");
    m.def("set_config", &set_config, "Set a config for the crossbar.");
    m.def("update_config", &update_config_pb,
          "Update configuration from JSON string.");
    m.def("gd_p", &get_gd_p_pb,
          "Get the positive (digital) conductance matrix.");
    m.def("gd_m", &get_gd_m_pb,
          "Get the negative (digital) conductance matrix.");
    m.def("ia_p", &get_ia_p_pb,
          "Get the positive (analog) conductance matrix.");
    m.def("ia_m", &get_ia_m_pb,
          "Get the negative (analog) conductance matrix.");
    m.def("cycles_p", &get_cycles_p_pb,
          "Get the number of reset-set cycles of the positive matrix.");
    m.def("cycles_m", &get_cycles_m_pb,
          "Get the number of reset-set cycles of the negative matrix.");
    m.def("write_ops", &get_write_xbar_counter,
          "Get the number of write operations.");
    m.def("mvm_ops", &get_mvm_counter,
          "Get the number of matrix-vector multiplication operations.");
    m.def("read_ops", &get_read_num,
          "Get the current number of consecutive read operations.");
}
