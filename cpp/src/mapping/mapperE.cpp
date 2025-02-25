/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "mapping/mapperE.h"
#include "helper/config.h"

namespace nq {

MapperE::MapperE() : Mapper(false) {
    // Calculate of the delta factor
    delta_ = 0.0;
    if (CFG.m_mode == INT8MappingMode::I_UINT_W_OFFS) {
        int curr_w_bit = CFG.W_BIT;
        for (size_t i = 0; i < CFG.SPLIT.size(); ++i) {
            shift_[i] = curr_w_bit - CFG.SPLIT[i];
            curr_w_bit -= CFG.SPLIT[i];
            delta_ += (1 << shift_[i]) * ((1 << CFG.SPLIT[i]) - 1);
        }
        delta_ = CFG.HRS / i_mm_ * delta_;
    }
}

MapperE::~MapperE() {}

void MapperE::d_write(const int32_t *mat, int32_t m_matrix, int32_t n_matrix) {
    d_write_offs(mat, m_matrix, n_matrix);
}

void MapperE::a_write(int32_t m_matrix, int32_t n_matrix) {
    a_write_p(m_matrix, n_matrix);
}

void MapperE::d_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                    int32_t m_matrix, int32_t n_matrix) {
    // The splitted matrix is of size CFG.SPLITsize*M x N (CFG.SPLITsize values
    // per original matrix value) Only one matrix exist: gd+ (gd_p_) The input
    // values 'vec' are stored in int32_t and they are all positive
    const std::vector<uint32_t> &split = CFG.SPLIT;
    const uint32_t tmp_size = m_matrix * split.size();
    std::vector<int32_t> tmp_out(tmp_size, 0);

    // Calculate sum over all inputs
    int64_t inp_sum = 0;
    for (size_t n = 0; n < n_matrix; ++n) {
        inp_sum += vec[n];
    }

    for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            tmp_out[t_m] += gd_p_[t_m][n] * vec[n];
        }
    }

    // Add sums caused by splitted weights
    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t s = 0; s < split.size(); ++s) {
            res[m] += tmp_out[m * split.size() + s] << shift_[s];
        }
        // Subtract term of compile-time constant
        res[m] -= inp_sum << (CFG.W_BIT - 1);
    }
}

void MapperE::a_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                    int32_t m_matrix, int32_t n_matrix) {
    // The splitted matrix is of size CFG.SPLITsize*M x N (CFG.SPLITsize values
    // per original matrix value) Only one matrix exist: ia+ (ia_p_) The input
    // is already positive only
    const std::vector<uint32_t> &split = CFG.SPLIT;
    const uint32_t tmp_size = m_matrix * split.size();
    std::vector<float> tmp_out(tmp_size, 0);
    std::vector<float> res_fp(m_matrix, 0);

    // Calculate sum over all inputs
    int64_t inp_sum = 0;
    for (size_t n = 0; n < n_matrix; ++n) {
        inp_sum += vec[n];
    }

    // For each bit in vec execute one MVM operation with ia_p_
    for (size_t i_bit = 0; i_bit < CFG.I_BIT; ++i_bit) {
        for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                tmp_out[t_m] += ia_p_[t_m][n] * ((vec[n] >> i_bit) & 1);
            }
        }

        // Addition of the partial results caused by splitted weights
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t s = 0; s < split.size(); ++s) {
                // No rounding is done here, so multiply instead of shift
                // tmp_out / i_step_size_[s] is a floating-point value
                res_fp[m] += static_cast<int32_t>(
                    round(adc_->analog_digital_conversion(
                              tmp_out[m * split.size() + s]) /
                          i_step_size_[s] * std::pow(2, shift_[s]) *
                          std::pow(2, i_bit)));
            }
        }

        // Reset tmp_out vector
        std::fill(tmp_out.begin(), tmp_out.end(), 0);
    }

    // Rescaling of the results and rounding
    for (size_t m = 0; m < m_matrix; ++m) {
        res_fp[m] -= inp_sum * (delta_ + (1 << (CFG.W_BIT - 1)));
        res[m] += static_cast<int32_t>(round(res_fp[m]));
    }
}

} // namespace nq