/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "mapping/int_mapper/int_v.h"
#include "helper/config.h"
#include <algorithm>

namespace nq {

MapperIntV::MapperIntV() :
    vd_p_(CFG.N, 0),
    tmp_out_int_(CFG.M * CFG.SPLIT.size(), 0),
    tmp_out_fp_(CFG.M * CFG.SPLIT.size(), 0.0),
    res_fp_(CFG.M * CFG.SPLIT.size(), 0.0),
    vd_slice_(CFG.N, 0),
    Mapper(false) {
    // Calculation of the delta factor
    delta_ = 0.0;
    if (CFG.m_mode == MappingMode::I_UINT_W_OFFS) {
        int curr_w_bit = CFG.W_BIT;
        for (size_t i = 0; i < CFG.SPLIT.size(); ++i) {
            shift_[i] = curr_w_bit - CFG.SPLIT[i];
            curr_w_bit -= CFG.SPLIT[i];
            delta_ += (1 << shift_[i]) * ((1 << CFG.SPLIT[i]) - 1);
        }
        delta_ = CFG.HRS / i_mm_ * delta_;
    }
}

MapperIntV::~MapperIntV() {}

void MapperIntV::d_write(const int32_t *mat, int32_t m_matrix,
                         int32_t n_matrix) {
    d_write_offs(mat, m_matrix, n_matrix);
}

void MapperIntV::a_write(int32_t m_matrix, int32_t n_matrix) {
    a_write_p(m_matrix, n_matrix);

    // Set conductance matrix of parasitic solver
    if (CFG.parasitics) {
        par_solver_->set_conductance_matrix(ia_p_, m_matrix * CFG.SPLIT.size(),
                                            n_matrix);
    }
}

void MapperIntV::d_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                       int32_t m_matrix, int32_t n_matrix) {
    // The splitted matrix is of size CFG.SPLITsize*M x N (CFG.SPLITsize values
    // per original matrix value) Only one matrix exist: gd+ (gd_p_) The input
    // values 'vec' are stored in int32_t and they are all positive
    const std::vector<uint32_t> &split = CFG.SPLIT;
    const uint32_t tmp_size = m_matrix * split.size();
    std::fill(tmp_out_int_.begin(), tmp_out_int_.end(), 0);

    //
    // Calculate sum over all inputs
    int64_t inp_sum = 0;
    for (size_t n = 0; n < n_matrix; ++n) {
        inp_sum += vec[n];
    }

    for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            tmp_out_int_[t_m] += gd_p_[t_m][n] * vec[n];
        }
    }

    // Add sums caused by splitted weights
    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t s = 0; s < split.size(); ++s) {
            res[m] += tmp_out_int_[m * split.size() + s] << shift_[s];
        }
        // Subtract term of compile-time constant
        res[m] -= inp_sum << (CFG.W_BIT - 1);
    }
}

void MapperIntV::a_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                       int32_t m_matrix, int32_t n_matrix) {
    // The splitted matrix is of size CFG.SPLITsize*M x N (CFG.SPLITsize values
    // per original matrix value) Only one matrix exist: ia+ (ia_p_) The input
    // is already positive only
    const std::vector<uint32_t> &split = CFG.SPLIT;
    const uint32_t tmp_size = m_matrix * split.size();
    std::fill(tmp_out_fp_.begin(), tmp_out_fp_.end(), 0.0);
    std::fill(res_fp_.begin(), res_fp_.end(), 0.0);

    // Construct input vector and calculate sum over all inputs
    int64_t inp_sum = 0;
    for (size_t n = 0; n < n_matrix; ++n) {
        vd_p_[n] = vec[n];
        inp_sum += vec[n];
    }

    // For each bit in vec execute one MVM operation with ia_p_
    for (size_t i_bit = 0; i_bit < CFG.I_BIT; ++i_bit) {
        // Slice input vector
        slice_vd(vd_p_, vd_slice_, n_matrix, i_bit);
        if (!CFG.parasitics) {
            for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
                for (size_t n = 0; n < n_matrix; ++n) {
                    tmp_out_fp_[t_m] += ia_p_[t_m][n] * vd_slice_[n];
                }
            }
        } else {
            par_solver_->compute_currents(vd_slice_, tmp_out_fp_, tmp_size,
                                          n_matrix);
        }

        // Addition of the partial results caused by splitted weights
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t s = 0; s < split.size(); ++s) {
                // No rounding is done here, so multiply instead of shift
                // tmp_out / i_step_size_[s] is a floating-point value
                res_fp_[m] += adc_->convert(
                    tmp_out_fp_[m * split.size() + s],
                    (std::pow(2, shift_[s]) * std::pow(2, i_bit)) /
                        i_step_size_[s]);
            }
        }

        // Reset tmp_out vector
        std::fill(tmp_out_fp_.begin(), tmp_out_fp_.end(), 0);
    }

    // Rescaling of the results and rounding
    for (size_t m = 0; m < m_matrix; ++m) {
        res_fp_[m] -= inp_sum * (delta_ + (1 << (CFG.W_BIT - 1)));
        res[m] += static_cast<int32_t>(round(res_fp_[m]));
    }
}

} // namespace nq
