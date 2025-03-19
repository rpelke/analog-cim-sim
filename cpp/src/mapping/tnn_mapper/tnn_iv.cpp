/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "mapping/tnn_mapper/tnn_iv.h"
#include "helper/config.h"

namespace nq {

MapperTnnIV::MapperTnnIV() :
    vd_p_(CFG.N, 0), vd_m_(CFG.N, 0), tmp_out_(CFG.M, 0.0),
    tmp_out_fp_(CFG.M, 0.0), Mapper(false) {}

MapperTnnIV::~MapperTnnIV() {}

void MapperTnnIV::d_write(const int32_t *mat, int32_t m_matrix,
                          int32_t n_matrix) {
    d_write_tc_tnn(mat, m_matrix, n_matrix, false);
}

void MapperTnnIV::a_write(int32_t m_matrix, int32_t n_matrix) {
    a_write_p_m_bnn_tnn(m_matrix, n_matrix);
}

void MapperTnnIV::d_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                        int32_t m_matrix, int32_t n_matrix) {
    const std::vector<uint32_t> &split = CFG.SPLIT;

    for (size_t n = 0; n < n_matrix; ++n) {
        if (vec[n] == +1) {
            vd_p_[n] = 1;
            vd_m_[n] = 0;
        } else if (vec[n] == -1) {
            vd_m_[n] = 1;
            vd_p_[n] = 0;
        } else if (vec[n] == 0) {
            vd_m_[n] = 0;
            vd_p_[n] = 0;
        } else {
            std::cerr << "TNN input is neither 0 nor +1 nor -1.";
            abort();
        }
    }

    if (split == std::vector<uint32_t>{1, 1}) {
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                res[m] += gd_p_[m][n] * (vd_p_[n] - vd_m_[n]) -
                          (gd_m_[m][n] * (vd_p_[n] - vd_m_[n]) << 1);
            }
        }
    } else {
        std::cerr << "Not implemented: SPLIT must be {1, 1} for TNN_IV."
                  << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

void MapperTnnIV::a_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                        int32_t m_matrix, int32_t n_matrix) {
    const std::vector<uint32_t> &split = CFG.SPLIT;
    std::fill(tmp_out_fp_.begin(), tmp_out_fp_.end(), 0.0);

    // Calculate sum over all inputs
    int64_t inp_sum = 0;
    for (size_t n = 0; n < n_matrix; ++n) {
        inp_sum += vec[n];
    }

    for (size_t n = 0; n < n_matrix; ++n) {
        if (vec[n] == +1) {
            vd_p_[n] = 1;
            vd_m_[n] = 0;
        } else if (vec[n] == -1) {
            vd_m_[n] = 1;
            vd_p_[n] = 0;
        } else if (vec[n] == 0) {
            vd_m_[n] = 0;
            vd_p_[n] = 0;
        } else {
            std::cerr << "TNN input is neither 0 nor +1 nor -1.";
            abort();
        }
    }

    // Analog correction term
    float analog_correction = inp_sum * CFG.HRS / i_mm_;

    if (split == std::vector<uint32_t>{1, 1}) {
        // LSB weights ia_p_ ; positive input
        std::fill(tmp_out_.begin(), tmp_out_.end(), 0.0);
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                tmp_out_[m] += ia_p_[m][n] * vd_p_[n];
            }
            tmp_out_fp_[m] +=
                adc_->analog_digital_conversion(tmp_out_[m]) / i_mm_;
        }

        // LSB weights ia_p_ ; negative input
        std::fill(tmp_out_.begin(), tmp_out_.end(), 0.0);
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                tmp_out_[m] += ia_p_[m][n] * vd_m_[n];
            }
            tmp_out_fp_[m] -=
                adc_->analog_digital_conversion(tmp_out_[m]) / i_mm_;
        }

        // MSB weights ia_m_ ; positive input
        std::fill(tmp_out_.begin(), tmp_out_.end(), 0.0);
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                tmp_out_[m] += ia_m_[m][n] * vd_p_[n];
            }
            tmp_out_fp_[m] -=
                adc_->analog_digital_conversion(tmp_out_[m]) * 2 / i_mm_;
        }

        std::fill(tmp_out_.begin(), tmp_out_.end(), 0.0);
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                tmp_out_[m] += ia_m_[m][n] * vd_m_[n];
            }
            tmp_out_fp_[m] +=
                adc_->analog_digital_conversion(tmp_out_[m]) * 2 / i_mm_;
        }

        for (size_t m = 0; m < m_matrix; ++m) {
            res[m] +=
                static_cast<int32_t>(round(tmp_out_fp_[m] + analog_correction));
        }
    } else {
        std::cerr << "Not implemented: SPLIT must be {1, 1} for TNN_IV."
                  << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

} // namespace nq