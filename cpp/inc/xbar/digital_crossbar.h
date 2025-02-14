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
#include <stdexcept>

#include "helper/definitions.h"

namespace nq {

class DigitalCrossbar {
    public :
        explicit DigitalCrossbar(uint32_t M, uint32_t N, uint32_t W_BIT, uint32_t I_BIT, std::vector<uint32_t> SPLIT, const bool digital_only, const INT8MappingMode m_mode);
        DigitalCrossbar() = delete;
        DigitalCrossbar(const DigitalCrossbar&) = delete;
        virtual ~DigitalCrossbar() = default;

        void write(const int32_t *mat, int32_t m_matrix, int32_t n_matrix);
        void mvm(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix, const std::vector<int32_t> *ct_const);
        const std::vector<std::vector<int32_t>>& get_gd_p() const;
        const std::vector<std::vector<int32_t>>& get_gd_m() const;
        int get_m_xbar() const;
        int get_n_xbar() const;
        const std::vector<int32_t>& get_ct_constants() const;

    private :
        void mvm_i_diff_w_diff(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix);
        void mvm_i_offs_w_diff(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix, const std::vector<int32_t> *ct_const);
        void mvm_i_tc_w_diff(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix);
        void mvm_i_uint_w_diff(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix);
        void mvm_i_uint_w_offs(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix);

        const uint32_t m_;
        const uint32_t n_;
        const uint32_t w_bit_;
        const uint32_t i_bit_;
        const std::vector<uint32_t> split_;
        std::vector<std::vector<int32_t>> gd_p_;
        std::vector<std::vector<int32_t>> gd_m_;
        std::vector<uint32_t> shift_;
        const bool digital_only_;
        const INT8MappingMode m_mode_;
        std::vector<int32_t> ct_constants_;
};

} // end namespace

#endif
