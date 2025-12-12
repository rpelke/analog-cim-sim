/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan                   *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/

#include <algorithm>
#include <execution>
#include <stdexcept>

#include "helper/config.h"
#include "helper/definitions.h"
#include "xbar/parasitics.h"

namespace nq {
ParasiticSolver::ParasiticSolver(const float wire_resistance,
                                 const float V_read) :
    m_xbar_(CFG.M),
    n_xbar_(CFG.N),
    w_res_(wire_resistance),
    v_read_(V_read),
    m_mode_(CFG.m_mode) {

    // Set encoding function pointers for different mapping modes. Throws error
    // if mapping mode is unsupported.
    if (mm_to_wenc.find(m_mode_) != mm_to_wenc.end()) {
        weight_enc_func_ = mm_to_wenc.at(m_mode_);
        input_enc_func_ = mm_to_ienc.at(m_mode_);
        output_enc_func_ = mm_to_oenc.at(m_mode_);
    } else {
        throw std::logic_error(
            "Unsupported mapping mode: " + m_mode_to_string(m_mode_) + "!");
    }
}

void ParasiticSolver::set_conductance_matrix(
    std::vector<std::vector<float>> &ia, int32_t m_matrix, int32_t n_matrix) {
    auto empty_vec = std::vector<std::vector<float>>{};
    set_conductance_matrix(ia, empty_vec, m_matrix, n_matrix);
}

void ParasiticSolver::set_conductance_matrix(
    std::vector<std::vector<float>> &ia_p,
    std::vector<std::vector<float>> &ia_m, int32_t m_matrix, int32_t n_matrix) {

    // Divide currents matrices with read voltage to get the actual conductance
    // values.
    auto div_v_read = [this](std::vector<std::vector<float>> &ia,
                             std::vector<std::vector<float>> &ga) -> void {
        ga.assign(this->m_xbar_ * CFG.SPLIT.size(),
                  std::vector<float>(this->n_xbar_, CFG.HRS));

        for (size_t m = 0; m < m_xbar_; m++) {
            for (size_t n = 0; n < n_xbar_; n++) {
                ga[m][n] = ia[m][n] / -(this->v_read_);
            }
        }
    };
    std::vector<std::vector<float>> ga_p, ga_m;
    std::optional<std::vector<std::vector<float>>> ga_m_opt = {};
    div_v_read(ia_p, ga_p);
    if (!ia_m.empty()) {
        div_v_read(ia_m, ga_m);
        ga_m_opt = ga_m;
    }

    // Encode weights and set conductance matrix
    (this->*weight_enc_func_)(ga_p, ga_m_opt, ga_mat_, m_matrix, n_matrix);

    // Set per column parasitic wire conductances
    ga_wire_ = std::vector<float>(ga_mat_[0].size(), (1 / w_res_) * 1e6);
}

void ParasiticSolver::compute_currents(std::vector<int32_t> &vd,
                                       std::vector<float> &res,
                                       int32_t m_matrix, int32_t n_matrix) {
    auto empty_vec = std::vector<int32_t>{};
    compute_currents(vd, empty_vec, res, m_matrix, n_matrix);
}

void ParasiticSolver::compute_currents(std::vector<int32_t> &vd_p,
                                       std::vector<int32_t> &vd_m,
                                       std::vector<float> &res,
                                       int32_t m_matrix, int32_t n_matrix) {

    // Encode input vectors for given mapping mode
    auto vd_m_opt = !vd_m.empty() ? std::optional<std::vector<int32_t>>{vd_m}
                                  : std::optional<std::vector<int32_t>>{};
    (this->*input_enc_func_)(vd_p, vd_m_opt, vd_, n_matrix);

    // Gate conductance matrix rows based on input voltage vector.
    size_t num_cols = ga_mat_[0].size();
    ga_mat_gated_.assign(n_xbar_, std::vector<float>(num_cols, 0));
    for (size_t n = 0; n < n_xbar_; n++) {
        if (vd_[n])
            ga_mat_gated_[n] = ga_mat_[n];
    }

    // Calculate column conductances
    ga_col_acc_.assign(num_cols, 0);

    // Lambdas to accumulate column conductances. Each row is parallel to
    // the previous columns and parasitic wires are in series to the the
    // columns. Result is stored in vector a.
    auto reduce_par_g = [](std::vector<float> &a, std::vector<float> &b,
                           size_t len) {
        std::transform(std::execution::par_unseq, a.begin(), a.begin() + len,
                       b.begin(), a.begin(),
                       [](const float &a, const float &b) { return a + b; });
    };
    auto reduce_ser_g = [](std::vector<float> &a, std::vector<float> &b,
                           size_t len) {
        std::transform(
            std::execution::par_unseq, a.begin(), a.begin() + len, b.begin(),
            a.begin(),
            [](const float &a, const float &b) { return (a * b) / (a + b); });
    };

    for (auto ga_row : ga_mat_gated_) {
        reduce_par_g(ga_col_acc_, ga_row, num_cols);
        reduce_ser_g(ga_col_acc_, ga_wire_, num_cols);
    }

    // Compute output currents.
    (this->*output_enc_func_)(ga_col_acc_, res, m_matrix);
}

void ParasiticSolver::w_enc_single(
    std::vector<std::vector<float>> &ga_p,
    std::optional<std::vector<std::vector<float>>> &ga_m,
    std::vector<std::vector<float>> &ga, int32_t m_matrix, int32_t n_matrix) {

    // Use only positive conductance matrix and encode each conductance in a
    // cell. Convert to row-wise vectors for more efficient indexing in the
    // solver.
    // TODO: Maybe add an assertion here to check XBar size.
    ga.assign(n_xbar_, std::vector<float>(m_matrix, 0));
    for (size_t m = 0; m < m_matrix; m++) {
        auto ga_col = ga_p[m];
        for (size_t n = 0; n < n_matrix; n++) {
            ga[n][m] = ga_col[n];
        }
    }
}

void ParasiticSolver::w_enc_double_col(
    std::vector<std::vector<float>> &ga_p,
    std::optional<std::vector<std::vector<float>>> &ga_m,
    std::vector<std::vector<float>> &ga, int32_t m_matrix, int32_t n_matrix) {
    // Interleave columns of positive (plus) and negative (minus) conductance
    // matrices and store into conductance matrix. Additionally, conductance
    // matrix is stored as row-wise vectors instead of column-wise vectors for
    // more efficient indexing in the solver.
    // TODO: Maybe add an assertion here to check XBar size.
    ga.assign(n_xbar_, std::vector<float>(m_matrix * 2, 0));
    for (size_t m = 0; m < m_matrix; m++) {
        auto ga_p_col = ga_p[m];
        auto ga_m_col = ga_m.value()[m];
        for (size_t n = 0; n < n_matrix; n++) {
            ga[n][2 * m] = ga_p_col[n];
            ga[n][2 * m + 1] = ga_m_col[n];
        }
    }
}

void ParasiticSolver::w_enc_double_row(
    std::vector<std::vector<float>> &ga_p,
    std::optional<std::vector<std::vector<float>>> &ga_m,
    std::vector<std::vector<float>> &ga, int32_t m_matrix, int32_t n_matrix) {
    // Interleave similar to double column but across rows.
    // XBar is assumed to have enough rows to accomodate this.
    // TODO: Maybe add an assertion here to check XBar size.
    ga.assign(n_xbar_, std::vector<float>(m_matrix, 0));
    for (size_t m = 0; m < m_matrix; m++) {
        auto ga_p_col = ga_p[m];
        auto ga_m_col = ga_m.value()[m];
        for (size_t n = 0; n < n_matrix; n++) {
            ga[2 * n][m] = ga_p_col[n];
            ga[2 * n + 1][m] = ga_m_col[n];
        }
    }
}

void ParasiticSolver::w_enc_quad(
    std::vector<std::vector<float>> &ga_p,
    std::optional<std::vector<std::vector<float>>> &ga_m,
    std::vector<std::vector<float>> &ga, int32_t m_matrix, int32_t n_matrix) {

    // Pair of conductances are diagonally mirrored on 4 neighbouring cells.
    // XBar is assumed to have enough rows/cols to accomodate this.
    // TODO: Maybe add an assertion here to check XBar size.
    ga.assign(n_xbar_, std::vector<float>(m_matrix * 2, 0));
    for (size_t m = 0; m < m_matrix; m++) {
        auto ga_p_col = ga_p[m];
        auto ga_m_col = ga_m.value()[m];
        for (size_t n = 0; n < n_matrix; n++) {
            ga[2 * n][2 * m] = ga_p_col[n];
            ga[2 * n + 1][2 * m] = ga_m_col[n];
            ga[2 * n][2 * m + 1] = ga_m_col[n];
            ga[2 * n + 1][2 * m + 1] = ga_p_col[n];
        }
    }
}

void ParasiticSolver::i_enc_single(std::vector<int32_t> &vd_p,
                                   std::optional<std::vector<int32_t>> &vd_m,
                                   std::vector<int32_t> &vd, int32_t n_matrix) {
    vd.assign(n_xbar_, 0);
    std::copy(vd_p.begin(), vd_p.begin() + n_matrix, vd.begin());
}

void ParasiticSolver::i_enc_double(std::vector<int32_t> &vd_p,
                                   std::optional<std::vector<int32_t>> &vd_m,
                                   std::vector<int32_t> &vd, int32_t n_matrix) {
    vd.assign(n_xbar_, 0);
    for (int n = 0; n < n_matrix; n++) {
        vd[2 * n] = vd_p[n];
        vd[2 * n + 1] = vd_m.value()[n];
    }
}

void ParasiticSolver::o_enc_single(std::vector<float> &tmp_res,
                                   std::vector<float> &res, int32_t m_matrix) {
    res.assign(m_xbar_, 0);
    for (size_t m = 0; m < m_matrix; m++) {
        res[m] = tmp_res[m] * -v_read_; // Assuming negative read voltage
    }
}

void ParasiticSolver::o_enc_double(std::vector<float> &tmp_res,
                                   std::vector<float> &res, int32_t m_matrix) {
    res.assign(m_xbar_, 0);
    for (size_t m = 0; m < m_matrix; m++) {
        res[m] = (tmp_res[2 * m] - tmp_res[2 * m + 1]) *
                 -v_read_; // Assuming negative read voltage
    }
}
} // namespace nq
