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

MapperBnnIV::MapperBnnIV() : Mapper(false) {
    if (CFG.SPLIT.size() != 1) {
        throw std::runtime_error("BNN_IV needs a split size of 1.");
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
    std::vector<int32_t> vd_p(n_matrix, 0);
    std::vector<int32_t> vd_m(n_matrix, 0);
    int32_t vec_sum = 0;

    for (size_t n = 0; n < n_matrix; ++n) {
        vec_sum += vec[n];
        if (vec[n] == +1) {
            vd_p[n] = 1;
        } else if (vec[n] == -1) {
            vd_m[n] = 1;
        } else {
            throw std::runtime_error("BNN input is neither +1 nor -1.");
        }
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            res[m] += 2 * (gd_p_[m][n] * vd_m[n] - gd_p_[m][n] * vd_p[n]);
        }
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        res[m] += vec_sum;
    }
}

void MapperBnnIV::a_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                        int32_t m_matrix, int32_t n_matrix) {
    std::vector<float> tmp_out(m_matrix, 0);
    std::vector<float> tmp_out_p(m_matrix, 0);
    std::vector<float> tmp_out_m(m_matrix, 0);
    std::vector<int32_t> vd_p(n_matrix, 0);
    std::vector<int32_t> vd_m(n_matrix, 0);
    int32_t vec_sum = 0;

    for (size_t n = 0; n < n_matrix; ++n) {
        vec_sum += vec[n];
        if (vec[n] == +1) {
            vd_p[n] = 1;
        } else if (vec[n] == -1) {
            vd_m[n] = 1;
        } else {
            throw std::runtime_error("BNN input is neither +1 nor -1.");
        }
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            tmp_out_p[m] += (ia_p_[m][n] * vd_p[n]);
            tmp_out_m[m] += (ia_p_[m][n] * vd_m[n]);
        }
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        tmp_out[m] += 2 / i_mm_ *
                      (adc_->analog_digital_conversion(tmp_out_m[m]) -
                       adc_->analog_digital_conversion(tmp_out_p[m]));
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        res[m] += round(tmp_out[m] + vec_sum + vec_sum * 2 * CFG.HRS / i_mm_);
    }
}

} // namespace nq