/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This work is licensed under the terms described in the LICENSE file        *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "mapping/bnn_mapper/bnn_vi.h"
#include "helper/config.h"

#include <iostream>

namespace nq {

MapperBnnVI::MapperBnnVI() :
    vd_p_(CFG.capacity().n, 0),
    vd_m_(CFG.capacity().n, 0),
    tmp_out_(CFG.capacity().m, 0.0),
    Mapper(PROPERTIES) {}

MapperBnnVI::~MapperBnnVI() {}

void MapperBnnVI::d_write(const int32_t *mat, int32_t m_matrix,
                          int32_t n_matrix) {
    d_write_diff_bnn(mat, m_matrix, n_matrix);

    if (CFG.mvm_profile) {
        // Construct new MVM profile stratum
        std::optional<std::reference_wrapper<std::vector<std::vector<int32_t>>>>
            gd_m{std::ref(gd_m_)};
        mvm_prof_cur_strat_ = mvm_prof_strat_factory_->get_stratum(
            std::map<std::string, float>{{"rows", n_matrix * 2},
                                         {"cols", m_matrix * 2},
                                         {"avg_cell_val", avg_cell_val_}});
    }
}

void MapperBnnVI::a_write(int32_t m_matrix, int32_t n_matrix) {
    a_write_p_m_bnn_tnn(m_matrix, n_matrix);

    // Set conductance matrix of parasitic solver
    if (CFG.parasitics) {
        par_solver_->set_conductance_matrix(ia_p_, ia_m_, CFG.state_columns(),
                                            CFG.capacity().n);
    }
}

void MapperBnnVI::d_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                        int32_t m_matrix, int32_t n_matrix) {
    for (size_t n = 0; n < n_matrix; ++n) {
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
            res[m] += gd_p_[m][n] * vd_p_[n] + gd_m_[m][n] * vd_m_[n] -
                      gd_m_[m][n] * vd_p_[n] - gd_p_[m][n] * vd_m_[n];
        }
    }
}

void MapperBnnVI::a_mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                        int32_t m_matrix, int32_t n_matrix,
                        const char *l_name) {
    std::fill(tmp_out_.begin(), tmp_out_.end(), 0.0);

    for (size_t n = 0; n < n_matrix; ++n) {
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

    if (!CFG.parasitics) {
        for (size_t m = 0; m < m_matrix; ++m) {
            for (size_t n = 0; n < n_matrix; ++n) {
                tmp_out_[m] += ia_p_[m][n] * vd_p_[n] + ia_m_[m][n] * vd_m_[n] -
                               ia_m_[m][n] * vd_p_[n] - ia_p_[m][n] * vd_m_[n];
            }
        }
    } else {
        par_solver_->compute_currents(vd_p_, vd_m_, tmp_out_, m_matrix,
                                      n_matrix);
    }

    adc_->convert(tmp_out_, tmp_out_, m_matrix, 1 / i_mm_, 0.0, l_name);

    for (size_t m = 0; m < m_matrix; ++m) {
        res[m] += tmp_out_[m];
    }

    if (CFG.mvm_profile) {
        // Profile inputs vd_p and vd_m as single MVM
        std::optional<std::reference_wrapper<std::vector<int32_t>>> vd_m{
            std::ref(vd_m_)};
        float avg_input_val = get_average_input_value(vd_p_, vd_m, n_matrix);
        profile_mvm(avg_input_val, l_name);
    }
}

} // namespace nq
