/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "mapping/crossbar_mapping.h"

namespace nq {

CrossbarMapping::CrossbarMapping(uint32_t M, uint32_t N, uint32_t W_BIT, uint32_t I_BIT,
std::vector<uint32_t> SPLIT, const bool digital_only, const INT8MappingMode m_mode,
const float HRS, const float LRS, const ADCType adc_type,
const float min_adc_curr, const float max_adc_curr, const float alpha,
const uint32_t resolution, const bool verbose) :
    d_xbar_(M, N, W_BIT, I_BIT, SPLIT, digital_only, m_mode),
    a_xbar_(M, N, W_BIT, I_BIT, SPLIT, HRS, LRS, m_mode, adc_type, min_adc_curr, max_adc_curr, alpha, resolution),
    i_bit_(I_BIT),
    split_(SPLIT),
    digital_only_(digital_only),
    m_mode_(m_mode),
    write_xbar_counter_(0),
    mvm_counter_(0),
    verbose_(verbose) {
}

void CrossbarMapping::write(const int32_t *mat, int32_t m_matrix, int32_t n_matrix) {
    write_xbar_counter_++;

    d_xbar_.write(mat, m_matrix, n_matrix);

    if (!digital_only_) {
        if ((m_mode_ == INT8MappingMode::I_DIFF_W_DIFF_1XB)
        || (m_mode_ == INT8MappingMode::I_DIFF_W_DIFF_2XB)
        || (m_mode_ == INT8MappingMode::I_TC_W_DIFF)
        || (m_mode_ == INT8MappingMode::I_TC_W_DIFF)
        || (m_mode_ == INT8MappingMode::I_UINT_W_DIFF)) {
            a_xbar_.write_p_m(d_xbar_.get_gd_p(), d_xbar_.get_gd_m(), m_matrix, n_matrix);
        }
        else if (m_mode_ == INT8MappingMode::I_UINT_W_OFFS) {
            a_xbar_.write_p(d_xbar_.get_gd_p(), m_matrix, n_matrix);
        }
        else {
            throw std::runtime_error("Mapping not implemented.");
        }
    }
}

void CrossbarMapping::mvm(int32_t *res, const int32_t *vec, const int32_t *mat, int32_t m_matrix, int32_t n_matrix) {
    mvm_counter_++;

    if (digital_only_) {
        d_xbar_.mvm(res, vec, nullptr, m_matrix, n_matrix, &d_xbar_.get_ct_constants());
    }
    else if ((m_mode_ == INT8MappingMode::I_DIFF_W_DIFF_1XB)
    || (m_mode_ == INT8MappingMode::I_DIFF_W_DIFF_2XB)
    || (m_mode_ == INT8MappingMode::I_TC_W_DIFF)
    || (m_mode_ == INT8MappingMode::I_TC_W_DIFF)
    || (m_mode_ == INT8MappingMode::I_UINT_W_DIFF)) {
        a_xbar_.mvm(res, vec, nullptr, m_matrix, n_matrix, nullptr);
    }
    else if (m_mode_ == INT8MappingMode::I_UINT_W_OFFS) {
        int32_t inp_sum = 0;
        for (size_t n = 0; n < n_matrix; ++n) {
            inp_sum += vec[n];
        }
        a_xbar_.mvm(res, vec, nullptr, m_matrix, n_matrix, nullptr, inp_sum);
    }
    else if ((m_mode_ == INT8MappingMode::I_OFFS_W_DIFF)) {
        a_xbar_.mvm(res, vec, nullptr, m_matrix, n_matrix, &d_xbar_.get_ct_constants());
    }
    else {
        throw std::runtime_error("Digital-to-analog mapping not implemented.");
    }
}

CrossbarMapping::~CrossbarMapping() {
    if (verbose_) {
        std::cout << "INT8MappingMode: " << m_mode_to_string(m_mode_) << std::endl;
        std::cout << "write_xbar_counter_: " << write_xbar_counter_ << std::endl;
        std::cout << "mvm_counter_: " << mvm_counter_ << std::endl;

        uint64_t num_write = 0;
        uint64_t num_mvm_total = 0;
        uint64_t num_mvm_sequential = 0;
        uint32_t cells_per_value = split_.size();

        switch (m_mode_) {
            case INT8MappingMode::I_DIFF_W_DIFF_1XB:
                num_write = write_xbar_counter_;
                num_mvm_total = mvm_counter_ * i_bit_;
                num_mvm_sequential = mvm_counter_ * 2 * i_bit_;
                cells_per_value *= 2;
                break;
            case INT8MappingMode::I_DIFF_W_DIFF_2XB:
                num_write = write_xbar_counter_ * 2;
                num_mvm_total = mvm_counter_ * 2 * i_bit_;
                num_mvm_sequential = mvm_counter_ * i_bit_;
                cells_per_value *= 4;
                break;
            case INT8MappingMode::I_OFFS_W_DIFF:
                num_write = write_xbar_counter_;
                num_mvm_total = mvm_counter_ * i_bit_;
                num_mvm_sequential = mvm_counter_ * i_bit_;
                cells_per_value *= 2;
                break;
            case INT8MappingMode::I_TC_W_DIFF:
                num_write = write_xbar_counter_;
                num_mvm_total = mvm_counter_ * i_bit_;
                num_mvm_sequential = mvm_counter_ * i_bit_;
                cells_per_value *= 2;
                break;
            case INT8MappingMode::I_UINT_W_DIFF:
                num_write = write_xbar_counter_;
                num_mvm_total = mvm_counter_ * i_bit_;
                num_mvm_sequential = mvm_counter_ * i_bit_;
                cells_per_value *= 2;
                break;
            case INT8MappingMode::I_UINT_W_OFFS:
                num_write = write_xbar_counter_;
                num_mvm_total = mvm_counter_ * i_bit_;
                num_mvm_sequential = mvm_counter_ * i_bit_;
                cells_per_value *= 1;
                break;
            default:
                throw std::runtime_error("Unknown mode.");
        }

        std::cout << "num_write: " << num_write << std::endl;
        std::cout << "num_mvm_total: " << num_mvm_total << std::endl;
        std::cout << "num_mvm_sequential: " << num_mvm_sequential << std::endl;
        std::cout << "cells_per_value: " << cells_per_value << std::endl;
    }
}

} // end namespace
