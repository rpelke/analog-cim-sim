/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "mapping/tnn_mapper/tnn_iii.h"
#include "helper/config.h"

namespace nq {

MapperTnnIII::MapperTnnIII() :
    vd_p_(CFG.N, 0), tmp_out_(CFG.M, 0.0), Mapper(true) {}

MapperTnnIII::~MapperTnnIII() {}

void MapperTnnIII::d_write(const int32_t *mat, int32_t m_matrix,
                           int32_t n_matrix) {
    d_write_diff_tnn(mat, m_matrix, n_matrix);
}

void MapperTnnIII::a_write(int32_t m_matrix, int32_t n_matrix) {
    a_write_p_m_bnn_tnn(m_matrix, n_matrix);
}

void MapperTnnIII::d_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                         int32_t m_matrix, int32_t n_matrix) {
    // Threat the input as two bit two's complement number
    // Input bit 0
    uint32_t mask = 0b01;
    for (size_t n = 0; n < n_matrix; ++n) {
        vd_p_[n] = (vec[n] + 1) & mask;
    }
    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            res[m] += (gd_p_[m][n] - gd_m_[m][n]) * vd_p_[n];
        }
    }

    // Input bit 1
    mask = 0b10;
    for (size_t n = 0; n < n_matrix; ++n) {
        vd_p_[n] = ((vec[n] + 1) & mask) >> 1;
    }
    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            res[m] += ((gd_p_[m][n] - gd_m_[m][n]) * vd_p_[n]) << 1;
        }
    }

    // Subtract digital offset
    for (size_t m = 0; m < m_matrix; ++m) {
        res[m] -= sum_w_[m];
    }
}

void MapperTnnIII::a_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                         int32_t m_matrix, int32_t n_matrix) {
    // Threat the input as two bit two's complement number.
    // Input bit 0
    uint32_t mask = 0b01;
    for (size_t n = 0; n < n_matrix; ++n) {
        vd_p_[n] = (vec[n] + 1) & mask;
    }
    std::fill(tmp_out_.begin(), tmp_out_.end(), 0.0);
    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            tmp_out_[m] += (ia_p_[m][n] - ia_m_[m][n]) * vd_p_[n];
        }
        res[m] += static_cast<int32_t>(
            round(adc_->analog_digital_conversion(tmp_out_[m]) / i_mm_));
    }

    // Input bit 1
    mask = 0b10;
    for (size_t n = 0; n < n_matrix; ++n) {
        vd_p_[n] = ((vec[n] + 1) & mask) >> 1;
    }
    std::fill(tmp_out_.begin(), tmp_out_.end(), 0.0);
    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            tmp_out_[m] += (ia_p_[m][n] - ia_m_[m][n]) * vd_p_[n];
        }
        res[m] += static_cast<int32_t>(
            round(adc_->analog_digital_conversion(tmp_out_[m]) * 2 / i_mm_));
    }

    // Subtract digital offset
    for (size_t m = 0; m < m_matrix; ++m) {
        res[m] -= sum_w_[m];
    }
}

} // namespace nq