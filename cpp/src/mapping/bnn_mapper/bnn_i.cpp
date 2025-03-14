/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "mapping/bnn_mapper/bnn_i.h"
#include "helper/config.h"

namespace nq {

MapperBnnI::MapperBnnI() : vd_(CFG.N, 0), tmp_out_(CFG.M, 0.0), Mapper(true) {
    if (CFG.SPLIT.size() != 1) {
        std::cerr << "BNN_I needs a split size of 1.";
        abort();
    }
}

MapperBnnI::~MapperBnnI() {}

void MapperBnnI::d_write(const int32_t *mat, int32_t m_matrix,
                         int32_t n_matrix) {
    d_write_diff_bnn(mat, m_matrix, n_matrix);
}

void MapperBnnI::a_write(int32_t m_matrix, int32_t n_matrix) {
    a_write_p_m_bnn(m_matrix, n_matrix);
}

void MapperBnnI::d_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                       int32_t m_matrix, int32_t n_matrix) {
    for (size_t n = 0; n < n_matrix; ++n) {
        vd_[n] = (vec[n] + 1) >> 1;
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            res[m] += ((gd_p_[m][n] - gd_m_[m][n]) * vd_[n]) * 2;
        }
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        res[m] -= sum_w_[m];
    }
}

void MapperBnnI::a_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                       int32_t m_matrix, int32_t n_matrix) {
    std::fill(tmp_out_.begin(), tmp_out_.end(), 0.0);

    for (size_t n = 0; n < n_matrix; ++n) {
        vd_[n] = (vec[n] + 1) >> 1;
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            tmp_out_[m] += (ia_p_[m][n] - ia_m_[m][n]) * vd_[n];
        }
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        res[m] +=
            round(adc_->analog_digital_conversion(tmp_out_[m]) * 2 / i_mm_) -
            sum_w_[m];
    }
}

} // namespace nq