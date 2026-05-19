/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Joel Klein, Arunkumar Vaidyanathan       *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This work is licensed under the terms described in the LICENSE file        *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include "oneapi/tbb.h"

#include "global_vars.h"
#include "helper/functions.h"
#include "interface_xbar.h"

/********************* Python interface *********************/
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

pybind11::array_t<uint64_t> get_consecutive_reads_p_pb() {
    check_xbar();

    const auto &consecutive_reads_p = xbar->get_consecutive_reads_p();
    size_t rows = consecutive_reads_p.size();
    size_t cols = (rows > 0) ? consecutive_reads_p[0].size() : 0;

    pybind11::array_t<uint64_t> result({rows, cols});
    auto r = result.mutable_unchecked<2>();
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            r(i, j) = consecutive_reads_p[i][j];
        }
    }
    return result;
}

pybind11::array_t<uint64_t> get_consecutive_reads_m_pb() {
    check_xbar();

    const auto &consecutive_reads_m = xbar->get_consecutive_reads_m();
    size_t rows = consecutive_reads_m.size();
    size_t cols = (rows > 0) ? consecutive_reads_m[0].size() : 0;

    pybind11::array_t<uint64_t> result({rows, cols});
    auto r = result.mutable_unchecked<2>();
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            r(i, j) = consecutive_reads_m[i][j];
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

/********************* Pybind definitions *********************/
PYBIND11_MODULE(acs_int, m) {
    m.def("cpy", &cpy_mtrx_pb, "Copy matrix to crossbar.");
    m.def("mvm", &exe_mvm_pb, "Execute matrix-vector multiplication.");
    m.def("set_config", &set_config, "Set a config for the crossbar.",
          pybind11::arg("cfg_file"), pybind11::arg("num_threads") = 1);
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
    m.def(
        "consecutive_reads_p", &get_consecutive_reads_p_pb,
        "Get the number of consecutive reads per cell of the positive matrix.");
    m.def(
        "consecutive_reads_m", &get_consecutive_reads_m_pb,
        "Get the number of consecutive reads per cell of the negative matrix.");
    m.def("write_ops", &get_write_xbar_counter,
          "Get the number of write operations.");
    m.def("mvm_ops", &get_mvm_counter,
          "Get the number of matrix-vector multiplication operations.");
    m.def("read_ops", &get_read_num,
          "Get the current number of consecutive read operations.");
    m.def("refresh_ops", &get_refresh_xbar_counter,
          "Get the number of refresh operations on the crossbar.");
    m.def("refresh_cell_ops", &get_refresh_cell_counter,
          "Get the number of refresh operations on the cells.");
    m.def("rd_run_out_of_bounds", &get_rd_run_out_of_bounds,
          "Check if the read disturb model ran out of bounds.");
    m.def("dump_adc_profile", &dump_adc_profile, "Dump ADC profile JSON file.");
}