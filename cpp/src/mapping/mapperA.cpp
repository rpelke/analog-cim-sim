/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "mapping/mapperA.h"
#include "helper/config.h"

namespace nq {

MapperA::MapperA() : Mapper(true) {}

MapperA::~MapperA() {}

void MapperA::d_write(const int32_t *mat, int32_t m_matrix, int32_t n_matrix) {
    d_write_diff(mat, m_matrix, n_matrix);
}

void MapperA::a_write(int32_t m_matrix, int32_t n_matrix) {
    a_write_p_m(m_matrix, n_matrix);
}

void MapperA::d_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                    int32_t m_matrix, int32_t n_matrix) {
    // The splitted matrix is of size CFG.SPLITsize*M x N (CFG.SPLITsize values
    // per original matrix value) Two matrices exist: gd+ (gd_p_) and gd-
    // (gd_m_) The input is also split into positive and negative values
    const std::vector<uint32_t> &split = CFG.SPLIT;
    const uint32_t tmp_size = m_matrix * split.size();
    std::vector<int32_t> tmp_out(tmp_size, 0);
    std::vector<int32_t> vd_p(n_matrix, 0);
    std::vector<int32_t> vd_m(n_matrix, 0);

    for (size_t n = 0; n < n_matrix; ++n) {
        if (vec[n] >= 0) {
            vd_p[n] = vec[n];
            vd_m[n] = 0;
        } else {
            vd_p[n] = 0;
            vd_m[n] = -vec[n];
        }
    }

    for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            tmp_out[t_m] += (gd_p_[t_m][n] - gd_m_[t_m][n]) * vd_p[n];
        }
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t s = 0; s < split.size(); ++s) {
            res[m] += tmp_out[m * split.size() + s] << shift_[s];
        }
    }

    std::fill(tmp_out.begin(), tmp_out.end(), 0);

    for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            tmp_out[t_m] += (gd_m_[t_m][n] - gd_p_[t_m][n]) * vd_m[n];
        }
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t s = 0; s < split.size(); ++s) {
            res[m] += tmp_out[m * split.size() + s] << shift_[s];
        }
    }
}

void MapperA::a_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                    int32_t m_matrix, int32_t n_matrix) {
    // The splitted matrix is of size CFG.SPLITsize*M x N (CFG.SPLITsize values
    // per original matrix value) Two matrices exist: ia+ (ia_p_) and ia-
    // (ia_m_).
    const std::vector<uint32_t> &split = CFG.SPLIT;
    const uint32_t tmp_size = m_matrix * split.size();
    std::vector<float> tmp_out(tmp_size, 0);
    std::vector<int32_t> vd_p(n_matrix, 0);
    std::vector<int32_t> vd_m(n_matrix, 0);

    for (size_t n = 0; n < n_matrix; ++n) {
        if (vec[n] >= 0) {
            vd_p[n] = vec[n];
            vd_m[n] = 0;
        } else {
            vd_p[n] = 0;
            vd_m[n] = -vec[n];
        }
    }

    // For each bit in vd_p execute one MVM operation with ia_p_ and one with
    // ia_m_ For positive inputs vd_p: bit 7 is always 0 (sign bit) -> CFG.I_BIT
    // - 1 Subract both results in the analog domain
    for (size_t i_bit = 0; i_bit < CFG.I_BIT - 1; ++i_bit) {
        // Calculcate multiplications with negative and positive weights
        for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                tmp_out[t_m] +=
                    (ia_p_[t_m][n] - ia_m_[t_m][n]) * ((vd_p[n] >> i_bit) & 1);
            }
        }

        // Addition of the partial results caused by splitted weights
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t s = 0; s < split.size(); ++s) {
                res[m] += static_cast<int32_t>(
                    round(adc_->analog_digital_conversion(
                              tmp_out[m * split.size() + s]) /
                          i_step_size_[s] * std::pow(2, shift_[s]) *
                          std::pow(2, i_bit)));
            }
        }

        // Reset tmp_out vector
        std::fill(tmp_out.begin(), tmp_out.end(), 0);
    }

    // For each bit in vd_m execute one MVM operation with ia_p_ and one with
    // ia_m_ Subract both results in the analog domain
    for (size_t i_bit = 0; i_bit < CFG.I_BIT; ++i_bit) {
        // Calculcate multiplications with negative and positive weights
        for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                tmp_out[t_m] +=
                    (ia_m_[t_m][n] - ia_p_[t_m][n]) * ((vd_m[n] >> i_bit) & 1);
            }
        }

        // Addition of the partial results caused by splitted weights
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t s = 0; s < split.size(); ++s) {
                res[m] += static_cast<int32_t>(
                    round(adc_->analog_digital_conversion(
                              tmp_out[m * split.size() + s]) /
                          i_step_size_[s] * std::pow(2, shift_[s]) *
                          std::pow(2, i_bit)));
            }
        }

        // Reset tmp_out vector
        std::fill(tmp_out.begin(), tmp_out.end(), 0);
    }
}

} // namespace nq