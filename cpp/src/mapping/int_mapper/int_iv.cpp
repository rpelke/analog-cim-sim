/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "mapping/int_mapper/int_iv.h"
#include "helper/config.h"

namespace nq {

MapperIntIV::MapperIntIV() :
    tmp_out_int_(CFG.M * CFG.SPLIT.size(), 0),
    tmp_out_fp_(CFG.M * CFG.SPLIT.size(), 0.0),
    Mapper(true) {}

MapperIntIV::~MapperIntIV() {}

void MapperIntIV::d_write(const int32_t *mat, int32_t m_matrix,
                          int32_t n_matrix) {
    d_write_diff(mat, m_matrix, n_matrix);
}

void MapperIntIV::a_write(int32_t m_matrix, int32_t n_matrix) {
    a_write_p_m(m_matrix, n_matrix);
}

void MapperIntIV::d_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                        int32_t m_matrix, int32_t n_matrix) {
    // The splitted matrix is of size CFG.SPLITsize*M x N (CFG.SPLITsize values
    // per original matrix value) Two matrices exist: gd+ (gd_p_) and gd-
    // (gd_m_) In this case, the input values 'vec' are stored in int32_t but
    // they are all positive
    const std::vector<uint32_t> &split = CFG.SPLIT;
    const uint32_t tmp_size = m_matrix * split.size();
    std::fill(tmp_out_int_.begin(), tmp_out_int_.end(), 0);

    for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            tmp_out_int_[t_m] += (gd_p_[t_m][n] - gd_m_[t_m][n]) * vec[n];
        }
    }

    // Add sums caused by splitted weights
    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t s = 0; s < split.size(); ++s) {
            res[m] += tmp_out_int_[m * split.size() + s] << shift_[s];
        }
    }
}

void MapperIntIV::a_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                        int32_t m_matrix, int32_t n_matrix) {
    // The splitted matrix is of size CFG.SPLITsize*M x N (CFG.SPLITsize values
    // per original matrix value) Two matrices exist: ia+ (ia_p_) and ia-
    // (ia_m_) The input is already positive only
    const std::vector<uint32_t> &split = CFG.SPLIT;
    const uint32_t tmp_size = m_matrix * split.size();
    std::fill(tmp_out_fp_.begin(), tmp_out_fp_.end(), 0.0);

    // For each bit in vec execute one MVM operation with ia_p_ and one with
    // ia_m_ Execute all multiplications with all positive interpreted inputs
    // first.
    for (size_t i_bit = 0; i_bit < CFG.I_BIT; ++i_bit) {
        // Calculcate multiplications with negative and positive weights
        for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                tmp_out_fp_[t_m] +=
                    (ia_p_[t_m][n] - ia_m_[t_m][n]) * ((vec[n] >> i_bit) & 1);
            }
        }

        // Addition of the partial results caused by splitted weights
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t s = 0; s < split.size(); ++s) {
                res[m] += static_cast<int32_t>(
                    round(adc_->analog_digital_conversion(
                              tmp_out_fp_[m * split.size() + s]) /
                          i_step_size_[s] * std::pow(2, shift_[s]) *
                          std::pow(2, i_bit)));
            }
        }

        // Reset tmp_out vector
        std::fill(tmp_out_fp_.begin(), tmp_out_fp_.end(), 0);
    }
}

} // namespace nq