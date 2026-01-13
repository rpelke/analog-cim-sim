/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This work is licensed under the terms described in the LICENSE file        *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "mapping/int_mapper/int_iv.h"
#include "helper/config.h"
#include <algorithm>

namespace nq {

MapperIntIV::MapperIntIV() :
    vd_p_(CFG.N, 0),
    tmp_out_int_(CFG.M * CFG.SPLIT.size(), 0),
    tmp_out_fp_(CFG.M * CFG.SPLIT.size(), 0.0),
    vd_slice_(CFG.N, 0),
    Mapper(true) {}

MapperIntIV::~MapperIntIV() {}

void MapperIntIV::d_write(const int32_t *mat, int32_t m_matrix,
                          int32_t n_matrix) {
    d_write_diff(mat, m_matrix, n_matrix);
}

void MapperIntIV::a_write(int32_t m_matrix, int32_t n_matrix) {
    a_write_p_m(m_matrix, n_matrix);

    // Set conductance matrix of parasitic solver
    if (CFG.parasitics) {
        par_solver_->set_conductance_matrix(
            ia_p_, ia_m_, m_matrix * CFG.SPLIT.size(), n_matrix);
    }
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
                        int32_t m_matrix, int32_t n_matrix,
                        const char *l_name) {
    // The splitted matrix is of size CFG.SPLITsize*M x N (CFG.SPLITsize values
    // per original matrix value) Two matrices exist: ia+ (ia_p_) and ia-
    // (ia_m_) The input is already positive only
    const std::vector<uint32_t> &split = CFG.SPLIT;
    const uint32_t tmp_size = m_matrix * split.size();
    std::fill(tmp_out_fp_.begin(), tmp_out_fp_.end(), 0.0);

    // Construct input vector
    for (size_t n = 0; n < n_matrix; ++n) {
        vd_p_[n] = vec[n];
    }

    // For each bit in vec execute one MVM operation with ia_p_ and one with
    // ia_m_ Execute all multiplications with all positive interpreted inputs
    // first.
    for (size_t i_bit = 0; i_bit < CFG.I_BIT; ++i_bit) {
        // Slice input vector
        slice_vd(vd_p_, vd_slice_, n_matrix, i_bit);
        // Calculcate multiplications with negative and positive weights
        if (!CFG.parasitics) {
            for (size_t t_m = 0; t_m < tmp_size; ++t_m) {
                for (size_t n = 0; n < n_matrix; ++n) {
                    tmp_out_fp_[t_m] +=
                        (ia_p_[t_m][n] - ia_m_[t_m][n]) * vd_slice_[n];
                }
            }
        } else {
            par_solver_->compute_currents(vd_slice_, tmp_out_fp_, tmp_size,
                                          n_matrix);
        }

        // Addition of the partial results caused by splitted weights
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t s = 0; s < split.size(); ++s) {
                res[m] += adc_->convert(
                    tmp_out_fp_[m * split.size() + s],
                    (std::pow(2, shift_[s]) * std::pow(2, i_bit)) /
                        i_step_size_[s]);
            }
        }

        // Reset tmp_out vector
        std::fill(tmp_out_fp_.begin(), tmp_out_fp_.end(), 0);
    }
}

} // namespace nq
