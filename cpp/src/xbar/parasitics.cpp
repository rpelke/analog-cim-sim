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
    m_xbar(CFG.M), n_xbar(CFG.N), w_res(wire_resistance), v_read(V_read),
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
    std::vector<std::vector<float>> &ia_m, int32_t m_matrix, int32_t n_matrix) {

    // Divide currents matrices with read voltage to get the actual conductance
    // values.
    auto div_v_read = [this](std::vector<std::vector<float>> ia)
        -> std::vector<std::vector<float>> {
        std::vector<std::vector<float>> ga(
            this->m_xbar * CFG.SPLIT.size(),
            std::vector<float>(this->n_xbar, CFG.HRS));

        for (size_t m = 0; m < m_xbar; m++) {
            for (size_t n = 0; n < n_xbar; n++) {
                ga[m][n] = ia[m][n] / -(this->v_read);
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
    ga_mat.assign(n_xbar, std::vector<float>(ga_p.size() + ga_m.size(), 0));
    // Interleave each column of original conductances into row-wise vectors.
    for (size_t m = 0; m < m_matrix; m++) {
        auto ga_p_col = ga_p[m];
        auto ga_m_col = ga_m[m];
        for (size_t n = 0; n < n_matrix; n++) {
            ga_mat[n][2 * m] = ga_p_col[n];
            ga_mat[n][2 * m + 1] = ga_m_col[n];
        }
    }
}

void ParasiticSolver::compute_currents(std::vector<int32_t> &vd,
                                       std::vector<float> &res) {

    size_t num_cols = ga_mat[0].size();
    // Gate conductance matrix rows based on input voltage vector.
    std::vector<std::vector<float>> ga_mat_gated(
        n_xbar, std::vector<float>(num_cols, 0));
    for (size_t n = 0; n < n_xbar; n++) {
        if (vd[n])
            ga_mat_gated[n] = ga_mat[n];
    }

    // Convert per-column wire resistance into conductance in uS
    std::vector<float> ga_wire(num_cols, (1 / w_res) * 1e6);

    // Calculate column conductances
    std::vector<float> ga_col_acc(num_cols, 0);

    // Lambdas to accumulate column conductances. Each row is parallel to
    // the previous columns and parasitic wires are in series to the the
    // columns.
    auto reduce_par_g = [](std::vector<float> &a, std::vector<float> &b,
                           size_t len) {
#pragma omp parallel for
        for (size_t n = 0; n < len; n++) {
            a[n] = a[n] + b[n];
        }
    };

    auto reduce_ser_g = [](std::vector<float> &a, std::vector<float> &b,
                           size_t len) {
#pragma omp parallel for
        for (size_t n = 0; n < len; n++) {
            a[n] = (a[n] * b[n]) / (a[n] + b[n]);
        }
    };

    for (auto ga_row : ga_mat_gated) {
        reduce_par_g(ga_col_acc, ga_row, num_cols);
        reduce_ser_g(ga_col_acc, ga_wire, num_cols);
    }

    // Compute output currents. Substract adjacent output currents and
    // multiply by read voltage.
    res.resize(num_cols);
    for (size_t col = 0; col < num_cols / 2; col++) {
        res[col] = (ga_col_acc[2 * col] - ga_col_acc[2 * col + 1]) *
                   -v_read; // Assuming negative read voltage
    }
}

} // namespace nq
