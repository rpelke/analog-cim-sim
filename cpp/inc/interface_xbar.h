#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <vector>

#ifdef DEBUG_MODE
#include <cstdint>
#endif

#ifndef EXPORT_API
#define EXPORT_API __attribute__((visibility("default")))
#endif

// C interface functions
const void check_pointer(const size_t *const size);
const void check_xbar();
template <typename T>
const uint32_t num_matrix_elems(const std::vector<std::vector<T>> &mat);

extern "C" {

EXPORT_API void set_config(const char *cfg_file, const int num_threads = 1);
EXPORT_API int32_t update_config(const char *json_config,
                                 const char *l_name = "Unknown");
EXPORT_API int32_t exe_mvm(int32_t *res, int32_t *vec, int32_t *mat,
                           int32_t m_matrix, int32_t n_matrix,
                           const char *l_name = "Unknown");
EXPORT_API int32_t cpy_mtrx(int32_t *mat, int32_t m_matrix, int32_t n_matrix,
                            const char *l_name = "Unknown");
EXPORT_API const void *get_gd_p(size_t *size);
EXPORT_API const void *get_gd_m(size_t *size);
EXPORT_API const void *get_ia_p(size_t *size);
EXPORT_API const void *get_ia_m(size_t *size);
EXPORT_API const uint64_t get_write_xbar_counter();
EXPORT_API const uint64_t get_mvm_counter();
EXPORT_API const uint64_t get_read_num();
EXPORT_API const uint64_t get_refresh_xbar_counter();
EXPORT_API const uint64_t get_refresh_cell_counter();
EXPORT_API const bool get_rd_run_out_of_bounds();
}

// C++ interface functions
EXPORT_API const std::vector<std::vector<int32_t>> &get_gd_p();
EXPORT_API const std::vector<std::vector<int32_t>> &get_gd_m();
EXPORT_API const std::vector<std::vector<float>> &get_ia_p();
EXPORT_API const std::vector<std::vector<float>> &get_ia_m();
EXPORT_API const std::vector<std::vector<uint64_t>> &get_cycles_p();
EXPORT_API const std::vector<std::vector<uint64_t>> &get_cycles_m();
EXPORT_API const std::vector<std::vector<uint64_t>> &get_consecutive_reads_p();
EXPORT_API const std::vector<std::vector<uint64_t>> &get_consecutive_reads_m();
EXPORT_API const std::string get_adc_profile();
EXPORT_API const void dump_adc_profile(const std::string filename);
