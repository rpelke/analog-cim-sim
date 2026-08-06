/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan                   *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This work is licensed under the terms described in the LICENSE file        *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "mapping/bnn_mapper/bnn_i.h"
#include "helper/config.h"

namespace nq {

MapperBnnI::MapperBnnI() :
    vd_(CFG.capacity().n, 0),
    tmp_out_(CFG.capacity().m, 0.0),
    Mapper(PROPERTIES) {}

MapperBnnI::~MapperBnnI() {}

void MapperBnnI::d_write(const int32_t *mat, int32_t m_matrix,
                         int32_t n_matrix) {
    d_write_diff_bnn(mat, m_matrix, n_matrix);

    if (CFG.mvm_profile) {
        // Construct new MVM profile stratum
        std::optional<std::reference_wrapper<std::vector<std::vector<int32_t>>>>
            gd_m{std::ref(gd_m_)};
        float avg_cell_val =
            get_average_cell_value(gd_p_, gd_m, m_matrix, n_matrix, 0, 1);
        mvm_cur_strat_ = mvm_strat_factory_->get_stratum(
            std::map<std::string, float>{{"rows", m_matrix},
                                         {"cols", n_matrix},
                                         {"avg_cell_val", avg_cell_val}});
    }
}

void MapperBnnI::a_write(int32_t m_matrix, int32_t n_matrix) {
    a_write_p_m_bnn_tnn(m_matrix, n_matrix);

    // Set conductance matrix of parasitic solver
    if (CFG.parasitics) {
        par_solver_->set_conductance_matrix(ia_p_, ia_m_, CFG.state_columns(),
                                            CFG.capacity().n);
    }
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
                       int32_t m_matrix, int32_t n_matrix, const char *l_name) {
    std::fill(tmp_out_.begin(), tmp_out_.end(), 0.0);

    for (size_t n = 0; n < n_matrix; ++n) {
        vd_[n] = (vec[n] + 1) >> 1;
    }

    if (!CFG.parasitics) {
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                tmp_out_[m] += (ia_p_[m][n] - ia_m_[m][n]) * vd_[n];
            }
        }
    } else {
        par_solver_->compute_currents(vd_, tmp_out_, m_matrix, n_matrix);
    }

    adc_->convert(tmp_out_, tmp_out_, m_matrix, 2 / i_mm_, 0.0, l_name);

    for (size_t m = 0; m < m_matrix; ++m) {
        res[m] += tmp_out_[m] - sum_w_[m];
    }

    if (CFG.mvm_profile) {
        float avg_input_val =
            get_average_input_value(vd_, std::nullopt, n_matrix);
        profile_mvm(avg_input_val, l_name);
    }
}

} // namespace nq
