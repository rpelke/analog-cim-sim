/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "mapping/bnn_mapper/bnn_v.h"
#include "helper/config.h"

namespace nq {

MapperBnnV::MapperBnnV() :
    vd_p_(CFG.N, 0), vd_m_(CFG.N, 0), tmp_out_(CFG.M, 0.0), Mapper(false) {
    if (CFG.SPLIT.size() != 1) {
        throw std::runtime_error("BNN_V needs a split size of 1.");
    }
}

MapperBnnV::~MapperBnnV() {}

void MapperBnnV::d_write(const int32_t *mat, int32_t m_matrix,
                         int32_t n_matrix) {
    d_write_diff_bnn(mat, m_matrix, n_matrix);
}

void MapperBnnV::a_write(int32_t m_matrix, int32_t n_matrix) {
    a_write_p_m_bnn(m_matrix, n_matrix);
}

void MapperBnnV::d_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                       int32_t m_matrix, int32_t n_matrix) {
    for (size_t n = 0; n < n_matrix; ++n) {
        if (vec[n] == +1) {
            vd_p_[n] = 1;
            vd_m_[n] = 0;
        } else if (vec[n] == -1) {
            vd_m_[n] = 1;
            vd_p_[n] = 0;
        } else {
            throw std::runtime_error("BNN input is neither +1 nor -1.");
        }
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        res[m] -= n_matrix;
        for (size_t n = 0; n < n_matrix; ++n) {
            res[m] += (gd_p_[m][n] * vd_p_[n] - gd_m_[m][n] * vd_m_[n]) << 1;
        }
    }
}

void MapperBnnV::a_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                       int32_t m_matrix, int32_t n_matrix) {
    std::fill(tmp_out_.begin(), tmp_out_.end(), 0.0);

    for (size_t n = 0; n < n_matrix; ++n) {
        if (vec[n] == +1) {
            vd_p_[n] = 1;
            vd_m_[n] = 0;
        } else if (vec[n] == -1) {
            vd_m_[n] = 1;
            vd_p_[n] = 0;
        } else {
            throw std::runtime_error("BNN input is neither +1 nor -1.");
        }
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            tmp_out_[m] += ia_p_[m][n] * vd_p_[n] + ia_m_[m][n] * vd_m_[n];
        }
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        res[m] +=
            round(2 / i_mm_ * adc_->analog_digital_conversion(tmp_out_[m]) -
                  n_matrix - 2 * n_matrix * CFG.HRS / i_mm_);
    }
}

} // namespace nq