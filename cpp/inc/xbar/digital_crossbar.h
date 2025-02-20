/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef DIGITAL_CROSSBAR_H
#define DIGITAL_CROSSBAR_H

#include <cstdint>
#include <vector>

namespace nq {

class DigitalCrossbar {
  public:
    explicit DigitalCrossbar();
    DigitalCrossbar(const DigitalCrossbar &) = delete;
    virtual ~DigitalCrossbar() = default;

    void write(const int32_t *mat, int32_t m_matrix, int32_t n_matrix);
    void mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
             int32_t m_matrix, int32_t n_matrix,
             const std::vector<int32_t> *ct_const);
    const std::vector<std::vector<int32_t>> &get_gd_p() const;
    const std::vector<std::vector<int32_t>> &get_gd_m() const;
    int get_m_xbar() const;
    int get_n_xbar() const;
    const std::vector<int32_t> &get_ct_constants() const;

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
                           int32_t m_matrix, int32_t n_matrix);

    std::vector<std::vector<int32_t>> gd_p_;
    std::vector<std::vector<int32_t>> gd_m_;
    std::vector<uint32_t> shift_;
    std::vector<int32_t> ct_constants_;
};

} // namespace nq

#endif
