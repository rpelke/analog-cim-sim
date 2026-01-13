/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This work is licensed under the terms described in the LICENSE file        *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "mapping/tnn_mapper/tnn_ii.h"
#include "helper/config.h"

namespace nq {

MapperTnnII::MapperTnnII() :
    vd_p_(CFG.N, 0),
    tmp_out_(CFG.M, 0.0),
    Mapper(true) {}

MapperTnnII::~MapperTnnII() {}

void MapperTnnII::d_write(const int32_t *mat, int32_t m_matrix,
                          int32_t n_matrix) {
    d_write_diff_tnn(mat, m_matrix, n_matrix);
}

void MapperTnnII::a_write(int32_t m_matrix, int32_t n_matrix) {
    a_write_p_m_bnn_tnn(m_matrix, n_matrix);

    // Set conductance matrix of parasitic solver
    if (CFG.parasitics) {
        par_solver_->set_conductance_matrix(ia_p_, ia_m_, m_matrix, n_matrix);
    }
}

void MapperTnnII::d_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                        int32_t m_matrix, int32_t n_matrix) {
    // Threat the input as two bit two's complement number
    // Input bit 0
    for (size_t n = 0; n < n_matrix; ++n) {
        vd_p_[n] = (vec[n] != 0) ? 1 : 0;
    }
    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            res[m] += (gd_p_[m][n] - gd_m_[m][n]) * vd_p_[n];
        }
    }

    // Input bit 1
    for (size_t n = 0; n < n_matrix; ++n) {
        vd_p_[n] = (vec[n] == -1) ? 1 : 0;
    }
    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            res[m] -= ((gd_p_[m][n] - gd_m_[m][n]) * vd_p_[n]) << 1;
        }
    }
}

void MapperTnnII::a_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                        int32_t m_matrix, int32_t n_matrix,
                        const char *l_name) {
    // Threat the input as two bit two's complement number.

    // Input bit 0
    for (size_t n = 0; n < n_matrix; ++n) {
        vd_p_[n] = (vec[n] != 0) ? 1 : 0;
    }
    std::fill(tmp_out_.begin(), tmp_out_.end(), 0.0);
    if (!CFG.parasitics) {
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                tmp_out_[m] += (ia_p_[m][n] - ia_m_[m][n]) * vd_p_[n];
            }
        }
    } else {
        par_solver_->compute_currents(vd_p_, tmp_out_, m_matrix, n_matrix);
    }

    adc_->convert(tmp_out_, tmp_out_, 1 / i_mm_, 0.0, l_name);

    for (size_t m = 0; m < m_matrix; ++m) {
        res[m] += tmp_out_[m];
    }

    // Input bit 1
    for (size_t n = 0; n < n_matrix; ++n) {
        vd_p_[n] = (vec[n] == -1) ? 1 : 0;
    }
    std::fill(tmp_out_.begin(), tmp_out_.end(), 0.0);
    if (!CFG.parasitics) {
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                tmp_out_[m] += (ia_p_[m][n] - ia_m_[m][n]) * vd_p_[n];
            }
        }
    } else {
        par_solver_->compute_currents(vd_p_, tmp_out_, m_matrix, n_matrix);
    }

    adc_->convert(tmp_out_, tmp_out_, 2 / i_mm_, 0.0, l_name);

    for (size_t m = 0; m < m_matrix; ++m) {
        res[m] -= tmp_out_[m];
    }
}

} // namespace nq
