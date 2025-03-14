/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "mapping/bnn_mapper/bnn_iv.h"
#include "helper/config.h"

namespace nq {

MapperBnnIV::MapperBnnIV() :
    vd_p_(CFG.N, 0), vd_m_(CFG.N, 0), tmp_out_(CFG.M, 0.0),
    tmp_out_p_(CFG.M, 0.0), tmp_out_m_(CFG.M, 0.0), Mapper(false) {
    if (CFG.SPLIT.size() != 1) {
        std::cerr << "BNN_IV needs a split size of 1.";
        abort();
    }
}

MapperBnnIV::~MapperBnnIV() {}

void MapperBnnIV::d_write(const int32_t *mat, int32_t m_matrix,
                          int32_t n_matrix) {
    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            gd_p_[m][n] = (mat[n_matrix * m + n] - 1) / -2;
        }
    }
}

void MapperBnnIV::a_write(int32_t m_matrix, int32_t n_matrix) {
    a_write_p_bnn(m_matrix, n_matrix);
}

void MapperBnnIV::d_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                        int32_t m_matrix, int32_t n_matrix) {
    int32_t vec_sum = 0;

    for (size_t n = 0; n < n_matrix; ++n) {
        vec_sum += vec[n];
        if (vec[n] == +1) {
            vd_p_[n] = 1;
            vd_m_[n] = 0;
        } else if (vec[n] == -1) {
            vd_m_[n] = 1;
            vd_p_[n] = 0;
        } else {
            std::cerr << "BNN input is neither +1 nor -1.";
            abort();
        }
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        res[m] += vec_sum;
        for (size_t n = 0; n < n_matrix; ++n) {
            res[m] += 2 * (gd_p_[m][n] * vd_m_[n] - gd_p_[m][n] * vd_p_[n]);
        }
    }
}

void MapperBnnIV::a_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                        int32_t m_matrix, int32_t n_matrix) {
    std::fill(tmp_out_.begin(), tmp_out_.end(), 0.0);
    std::fill(tmp_out_p_.begin(), tmp_out_p_.end(), 0.0);
    std::fill(tmp_out_m_.begin(), tmp_out_m_.end(), 0.0);
    int32_t vec_sum = 0;

    for (size_t n = 0; n < n_matrix; ++n) {
        vec_sum += vec[n];
        if (vec[n] == +1) {
            vd_p_[n] = 1;
            vd_m_[n] = 0;
        } else if (vec[n] == -1) {
            vd_m_[n] = 1;
            vd_p_[n] = 0;
        } else {
            std::cerr << "BNN input is neither +1 nor -1.";
            abort();
        }
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            tmp_out_p_[m] += (ia_p_[m][n] * vd_p_[n]);
            tmp_out_m_[m] += (ia_p_[m][n] * vd_m_[n]);
        }
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        tmp_out_[m] += 2 / i_mm_ *
                       (adc_->analog_digital_conversion(tmp_out_m_[m]) -
                        adc_->analog_digital_conversion(tmp_out_p_[m]));
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        res[m] += round(tmp_out_[m] + vec_sum + vec_sum * 2 * CFG.HRS / i_mm_);
    }
}

} // namespace nq