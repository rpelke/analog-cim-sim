/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "mapping/bnn_mapper/bnn_ii.h"
#include "helper/config.h"

namespace nq {

MapperBnnII::MapperBnnII() :
    vd_(CFG.N, 0),
    tmp_out_(CFG.M, 0.0),
    Mapper(true) {}

MapperBnnII::~MapperBnnII() {}

void MapperBnnII::d_write(const int32_t *mat, int32_t m_matrix,
                          int32_t n_matrix) {
    d_write_diff_bnn(mat, m_matrix, n_matrix);
}

void MapperBnnII::a_write(int32_t m_matrix, int32_t n_matrix) {
    a_write_p_m_bnn_tnn(m_matrix, n_matrix);

    // Set conductance matrix of parasitic solver
    if (CFG.parasitics) {
        // Switch ia_p and ia_m for proper conductance encoding
        par_solver_->set_conductance_matrix(ia_m_, ia_p_, m_matrix, n_matrix);
    }
}

void MapperBnnII::d_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                        int32_t m_matrix, int32_t n_matrix) {
    for (size_t n = 0; n < n_matrix; ++n) {
        vd_[n] = (vec[n] - 1) / (-2);
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            res[m] += ((gd_m_[m][n] - gd_p_[m][n]) * vd_[n]) * 2;
        }
    }

    for (size_t m = 0; m < m_matrix; ++m) {
        res[m] += sum_w_[m];
    }
}

void MapperBnnII::a_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                        int32_t m_matrix, int32_t n_matrix) {
    std::fill(tmp_out_.begin(), tmp_out_.end(), 0.0);

    for (size_t n = 0; n < n_matrix; ++n) {
        vd_[n] = (vec[n] - 1) / (-2);
    }

    if (!CFG.parasitics) {
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                tmp_out_[m] += (ia_m_[m][n] - ia_p_[m][n]) * vd_[n];
            }
        }
    } else {
        par_solver_->compute_currents(vd_, tmp_out_, m_matrix, n_matrix);
    }

    adc_->convert(tmp_out_, tmp_out_, 2 / i_mm_, 0.0);

    for (size_t m = 0; m < m_matrix; ++m) {
        res[m] += tmp_out_[m] + sum_w_[m];
    }
}

} // namespace nq
