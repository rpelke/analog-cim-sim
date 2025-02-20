/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef ANALOG_CROSSBAR_H
#define ANALOG_CROSSBAR_H

#include <cstdint>
#include <memory>
#include <vector>

#include "adc/adc.h"

namespace nq {

class AnalogCrossbar {
  public:
    explicit AnalogCrossbar();
    AnalogCrossbar(const AnalogCrossbar &) = delete;
    virtual ~AnalogCrossbar() = default;

    void write_p_m(const std::vector<std::vector<int32_t>> &gd_p,
                   const std::vector<std::vector<int32_t>> &gd_m,
                   int32_t m_matrix, int32_t n_matrix);
    void write_p(const std::vector<std::vector<int32_t>> &gd_p,
                 int32_t m_matrix, int32_t n_matrix);
    void mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
             int32_t m_matrix, int32_t n_matrix,
             const std::vector<int32_t> *ct_const, int32_t inp_sum = 0);
    const std::vector<std::vector<float>> &get_ga_p() const;
    const std::vector<std::vector<float>> &get_ga_m() const;

  private:
    void mvm_i_diff_w_diff(int32_t *res, const int32_t *vec, const int32_t *mat,
                           int32_t m_matrix, int32_t n_matrix);
    void mvm_i_offs_w_diff(int32_t *res, const int32_t *vec, const int32_t *mat,
                           int32_t m_matrix, int32_t n_matrix,
                           const std::vector<int32_t> *ct_const);
    void mvm_i_tc_w_diff(int32_t *res, const int32_t *vec, const int32_t *mat,
                         int32_t m_matrix, int32_t n_matrix);
    void mvm_i_uint_w_diff(int32_t *res, const int32_t *vec, const int32_t *mat,
                           int32_t m_matrix, int32_t n_matrix);
    void mvm_i_uint_w_offs(int32_t *res, const int32_t *vec, const int32_t *mat,
                           int32_t m_matrix, int32_t n_matrix, int32_t inp_sum);

    std::vector<std::vector<float>> ia_p_;
    std::vector<std::vector<float>> ia_m_;
    std::vector<float> i_step_size_;
    int num_segments_;
    float i_mm_;
    std::vector<uint32_t> shift_;
    const std::unique_ptr<ADC> adc_;
    float delta_i_uint_w_offs_;
};

} // namespace nq

#endif
