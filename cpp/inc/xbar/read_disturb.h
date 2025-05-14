/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef READ_DISTURB_H
#define READ_DISTURB_H

#include <cstdint>
#include <vector>

namespace nq {

/*
This read disturb model is based on a paper written by Cheng-Min Jiang et al.:
"An Analytical Model of Read-Disturb Failure Time in a Post-Cycling Resistive
Switching Memory", Link: https://ieeexplore.ieee.org/document/9580480
*/
class ReadDisturb {
  public:
    ReadDisturb(const float V_read);
    ReadDisturb(const ReadDisturb &) = delete;
    virtual ~ReadDisturb() = default;

    const std::vector<std::vector<uint64_t>> &get_cycles_p() const;
    const std::vector<std::vector<uint64_t>> &get_cycles_m() const;
    void update_cycles(const std::vector<std::vector<bool>> &update_p,
                       const std::vector<std::vector<bool>> &update_m);
    float calc_G0_scaling_factor(const uint64_t read_num,
                                 const uint64_t N_cycles) const;
    float calc_transition_time(const uint64_t N_cycles) const;

  private:
    float calc_exp_tt(const float V_read) const;
    float calc_p(const float V_read) const;

    std::vector<std::vector<uint64_t>> cycles_p_;
    std::vector<std::vector<uint64_t>> cycles_m_;

    const float t0_;
    const float fitting_param_; // Obtained from papers graphs
    const float c1_;
    const float a_;
    const float kb_;
    const float T_;
    const float k_;
    const float m_;
    const float kb_T_;
    const float V_read_;
    const float exp_tt_; // Exponent for transition time calculation
    const float p_;      // Power factor
};

} // namespace nq

#endif
