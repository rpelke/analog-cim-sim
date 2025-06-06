/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "xbar/read_disturb.h"
#include "helper/config.h"

namespace nq {

ReadDisturb::ReadDisturb(const float V_read) :
    cycles_p_(CFG.M * CFG.SPLIT.size(), std::vector<uint64_t>(CFG.N, 0)),
    cycles_m_(CFG.M * CFG.SPLIT.size(), std::vector<uint64_t>(CFG.N, 0)),
    consecutive_reads_p_(CFG.M * CFG.SPLIT.size(),
                         std::vector<uint64_t>(CFG.N, 0)),
    consecutive_reads_m_(CFG.M * CFG.SPLIT.size(),
                         std::vector<uint64_t>(CFG.N, 0)),
    t0_(1.55e-8), fitting_param_(1.43339), c1_(0.0068), a_(0.11),
    kb_(1.38064852e-23), T_(300.0), k_(0.003), m_(0.41),
    kb_T_(kb_ * T_ / 1.602176634e-19), V_read_(V_read),
    exp_tt_(calc_exp_tt(V_read)), p_(calc_p(V_read)) {}

const std::vector<std::vector<uint64_t>> &ReadDisturb::get_cycles_p() const {
    return cycles_p_;
}

const std::vector<std::vector<uint64_t>> &ReadDisturb::get_cycles_m() const {
    return cycles_m_;
}

const std::vector<std::vector<uint64_t>> &
ReadDisturb::get_consecutive_reads_p() const {
    return consecutive_reads_p_;
}

const std::vector<std::vector<uint64_t>> &
ReadDisturb::get_consecutive_reads_m() const {
    return consecutive_reads_m_;
}

// Count the number of set-reset cycles for each cell
void ReadDisturb::update_cycles(
    const std::vector<std::vector<bool>> &update_p,
    const std::vector<std::vector<bool>> &update_m) {
    for (size_t i = 0; i < cycles_p_.size(); ++i) {
        for (size_t j = 0; j < cycles_p_[i].size(); ++j) {
            cycles_p_[i][j] += update_p[i][j];
            cycles_m_[i][j] += update_m[i][j];
        }
    }
}

float ReadDisturb::calc_G0_scaling_factor(const uint64_t read_num,
                                          const uint64_t N_cycles) const {
    float tt = calc_transition_time(N_cycles);
    float t_stress = read_num * CFG.t_read;
    if (t_stress < tt) {
        return 1.0f;
    } else {
        return std::pow((t_stress / tt), -p_);
    }
}

// Calculate the transition time based on the V_read of the config
float ReadDisturb::calc_transition_time(const uint64_t N_cycles) const {
    double exp = k_ * std::pow(N_cycles, m_);
    if (exp >= 1.0) {
        std::cerr
            << "Warning: N_cycle too big for read disturb model. Model not "
               "defined for N_cycles >= "
            << N_cycles << ". ";
        return 0.0f;
    }
    return t0_ * std::pow(fitting_param_, exp_tt_) *
           std::pow(1.0 - exp, exp_tt_);
}

// Calculate the exponent for the transition time
float ReadDisturb::calc_exp_tt(const float V_read) const {
    return 1 / (c1_ * std::exp(a_ * std::abs(V_read) / kb_T_));
}

// Calculate the power factor p
float ReadDisturb::calc_p(const float V_read) const {
    return c1_ * std::exp(a_ * std::abs(V_read) / kb_T_);
}

void ReadDisturb::update_cycle_p(int m, int n, uint64_t cycles) {
    cycles_p_[m][n] += cycles;
}

void ReadDisturb::update_cycle_m(int m, int n, uint64_t cycles) {
    cycles_m_[m][n] += cycles;
}

void ReadDisturb::update_consecutive_reads(int32_t m_matrix, int32_t n_matrix) {
    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            consecutive_reads_p_[m][n]++;
            consecutive_reads_m_[m][n]++;
        }
    }
}

void ReadDisturb::reset_all_consecutive_reads() {
    for (size_t m = 0; m < CFG.M * CFG.SPLIT.size(); ++m) {
        for (size_t n = 0; n < CFG.N; ++n) {
            consecutive_reads_p_[m][n] = 0;
            consecutive_reads_m_[m][n] = 0;
        }
    }
}

void ReadDisturb::reset_consecutive_reads_p(int m, int n) {
    consecutive_reads_p_[m][n] = 0;
}

void ReadDisturb::reset_consecutive_reads_m(int m, int n) {
    consecutive_reads_m_[m][n] = 0;
}

} // namespace nq