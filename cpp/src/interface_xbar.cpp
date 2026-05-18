/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Joel Klein, Arunkumar Vaidyanathan       *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This work is licensed under the terms described in the LICENSE file        *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/

#include "oneapi/tbb.h"

#include "helper/config.h"
#include "xbar/crossbar.h"

#ifndef EXPORT_API
#define EXPORT_API __attribute__((visibility("default")))
#endif
#include "../inc/global_vars.h"
#include "../inc/interface_xbar.h"

/********************** Global variables **********************/
bool cfg_loaded = nq::Config::get_cfg().load_cfg("");
std::unique_ptr<nq::Crossbar> xbar =
    (cfg_loaded) ? std::make_unique<nq::Crossbar>() : nullptr;
std::string adc_profile_cache = "";
std::unique_ptr<tbb::global_control> gc; /** TBB Global Control */

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
extern "C" EXPORT_API void set_config(const char *cfg_file,
                                      const int num_threads) {
    xbar = nullptr;
    nq::Config::get_cfg().load_cfg(cfg_file);
    xbar = std::make_unique<nq::Crossbar>();

    // Set maximum number of threads
    gc = std::make_unique<tbb::global_control>(
        tbb::global_control::max_allowed_parallelism, num_threads);
}

extern "C" EXPORT_API int32_t update_config(const char *json_config,
                                            const char *l_name) {
#ifdef DEBUG_MODE
    std::cout << "Update config" << std::endl;
    std::cout << "Layer: " << l_name << std::endl;
    std::cout << "Update config called with JSON: " << json_config << std::endl;
#endif
    if (xbar == nullptr) {
        std::cerr << "Error: Crossbar is not initialized. Please call "
                     "set_config() first."
                  << std::endl;
        return -1;
    }
    if (json_config == nullptr) {
        std::cerr << "Warning: Config JSON is null. No changes applied."
                  << std::endl;
        return -1;
    }

    // Track whether parameters requiring crossbar recreation were updated
    bool recreate_xbar = false;

    // Let Config class handle the JSON parsing and updates
    bool config_updated =
        nq::Config::get_cfg().update_cfg(json_config, &recreate_xbar);

    // Only recreate crossbar if necessary keys were updated
    if (config_updated && recreate_xbar) {
        xbar = std::make_unique<nq::Crossbar>();
    }
#ifdef DEBUG_MODE
    std::cout << "Config update completed." << std::endl;
#endif
    return 0;
}

extern "C" EXPORT_API int32_t exe_mvm(int32_t *res, int32_t *vec, int32_t *mat,
                                      int32_t m_matrix, int32_t n_matrix,
                                      const char *l_name) {
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
    xbar->mvm(res, vec, mat, m_matrix, n_matrix, l_name);
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
                                       int32_t n_matrix, const char *l_name) {
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

extern "C" EXPORT_API const uint64_t get_refresh_xbar_counter() {
    check_xbar();
    return xbar->get_refresh_xbar_counter();
}

extern "C" EXPORT_API const uint64_t get_refresh_cell_counter() {
    check_xbar();
    return xbar->get_refresh_cell_counter();
}

extern "C" EXPORT_API const bool get_rd_run_out_of_bounds() {
    check_xbar();
    return xbar->get_rd_run_out_of_bounds();
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

EXPORT_API const std::vector<std::vector<uint64_t>> &get_consecutive_reads_p() {
    return xbar->get_consecutive_reads_p();
}

EXPORT_API const std::vector<std::vector<uint64_t>> &get_consecutive_reads_m() {
    return xbar->get_consecutive_reads_m();
}

EXPORT_API const std::string get_adc_profile() {
    return nq::ADCHistograms::get_instance().to_json().dump();
}

EXPORT_API const void dump_adc_profile(const std::string filename) {
    std::ofstream file_stream(filename);
    if (!file_stream.is_open()) {
        std::cerr << "Could not open ADC profile dump file!";
        std::exit(EXIT_FAILURE);
    }
    file_stream << nq::ADCHistograms::get_instance().to_json().dump(2);
    file_stream.close();
}
