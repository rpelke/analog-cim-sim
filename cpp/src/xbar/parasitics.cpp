/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan                   *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/

#include <stdexcept>

#include "helper/config.h"
#include "helper/definitions.h"
#include "xbar/parasitics.h"

namespace nq {
ParasiticSolver::ParasiticSolver(const float wire_resistance,
                                 const float V_read) :
    m_matrix(CFG.M), n_matrix(CFG.N), w_res(wire_resistance), v_read(V_read),
    m_mode(CFG.m_mode) {

    // TODO: Current support limited to BNN_I mapping mode. Need to implement
    // for others.
    if (m_mode != MappingMode::BNN_I) {
        throw std::logic_error(
            "Unsupported mapping mode: " + m_mode_to_string(m_mode) + "!");
    }
}

void ParasiticSolver::set_conductance_matrix(
    std::vector<std::vector<float>> &ia_p,
    std::vector<std::vector<float>> &ia_m) {

    // Divide currents matrices with read voltage to get the actual conductance
    // values.
    auto div_v_read = [this](std::vector<std::vector<float>> ia)
        -> std::vector<std::vector<float>> {
        std::vector<std::vector<float>> ga(
            this->m_matrix * CFG.SPLIT.size(),
            std::vector<float>(this->n_matrix, CFG.HRS));

        for (size_t m = 0; m < m_matrix; m++) {
            for (size_t n = 0; n < n_matrix; n++) {
                ga[m][n] = ia[m][n] / this->v_read;
            }
        }
        return ga;
    };

    auto ga_p = div_v_read(ia_p);
    auto ga_m = div_v_read(ia_m);

    // Interleave columns of positive (plus) and negative (minus) conductance
    // matrices and store into conductance matrix. Additionally, conductance
    // matrix is stored as row-wise vectors instead of column-wise vectors for
    // more efficient indexing in the solver.
    ga_mat.assign(n_matrix, std::vector<float>());
    // Reserve space
    for (size_t n = 0; n < n_matrix; n++) {
        ga_mat[n].reserve(ga_p.size() + ga_m.size());
    }
    // Interleave each column of original conductances into row-wise vectors.
    for (auto ga_p_col = ga_p.begin(), ga_m_col = ga_m.begin();
         ga_p_col != ga_p.end() && ga_m_col != ga_m.end();
         ga_p_col++, ga_m_col++) {
        for (size_t n = 0; n < n_matrix; n++) {
            ga_mat[n].push_back((*ga_p_col)[n]);
            ga_mat[n].push_back((*ga_m_col)[n]);
        }
    }
}

void ParasiticSolver::compute_currents(std::vector<int32_t> &vd,
                                       std::vector<float> &res) {

    size_t num_cols = ga_mat.size();
    // Gate conductance matrix rows based on input voltage vector.
    std::vector<std::vector<float>> ga_mat_gated(
        n_matrix, std::vector<float>(num_cols, 0));
    for (size_t n = 0; n < n_matrix; n++) {
        if (vd[n])
            ga_mat_gated[n] = ga_mat[n];
    }

    // Convert per-column wire resistance into conductance in uS
    std::vector<float> ga_wire(num_cols, (1 / w_res) * 1e6);

    // Calculate column conductances
    std::vector<float> ga_col_acc(num_cols, 0);

    // Lambda to accumulate column conductances. Each row is parallel to
    // the previous columns and parasitic wires are in series to the the
    // columns.
    auto accumulate_ga_col = [&ga_col_acc, &ga_wire,
                              num_cols](std::vector<float> &ga_row) {
        float ga_col_row_parallel;

#pragma omp parallel for private(ga_col_row_parallel)
        for (size_t col = 0; col < num_cols; col++) {
            ga_col_row_parallel = ga_col_acc[col] + ga_row[col];
            ga_col_acc[col] = (ga_col_row_parallel * ga_wire[col]) /
                              (ga_col_row_parallel + ga_wire[col]);
        }
    };

    for (auto ga_row : ga_mat_gated) {
        accumulate_ga_col(ga_row);
    }

    // Compute output currents
    res.resize(num_cols);
    for (size_t col = 0; col < num_cols; col++) {
        res[col] = ga_col_acc[col] * v_read;
    }
}

} // namespace nq
