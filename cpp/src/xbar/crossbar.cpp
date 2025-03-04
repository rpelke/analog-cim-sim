/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "xbar/crossbar.h"
#include "helper/config.h"

namespace nq {

Crossbar::Crossbar() :
    mapper_(Mapper::create_from_config()), write_xbar_counter_(0),
    mvm_counter_(0) {}

void Crossbar::write(const int32_t *mat, int32_t m_matrix, int32_t n_matrix) {
    write_xbar_counter_++;
    mapper_->d_write(mat, m_matrix, n_matrix);
    if (!CFG.digital_only) {
        mapper_->a_write(m_matrix, n_matrix);
    }
}

void Crossbar::mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
                   int32_t m_matrix, int32_t n_matrix) {
    mvm_counter_++;
    if (CFG.digital_only) {
        mapper_->d_mvm(res, vec, mat, m_matrix, n_matrix);
    } else {
        mapper_->a_mvm(res, vec, mat, m_matrix, n_matrix);
    }
}

Crossbar::~Crossbar() {
    if (CFG.verbose) {
        std::cout << "MappingMode: " << m_mode_to_string(CFG.m_mode)
                  << std::endl;
        std::cout << "write_xbar_counter_: " << write_xbar_counter_
                  << std::endl;
        std::cout << "mvm_counter_: " << mvm_counter_ << std::endl;

        uint64_t num_write = 0;
        uint64_t num_mvm_total = 0;
        uint64_t num_mvm_sequential = 0;
        uint32_t cells_per_value = CFG.SPLIT.size();

        switch (CFG.m_mode) {
        case MappingMode::I_DIFF_W_DIFF_1XB:
            num_write = write_xbar_counter_;
            num_mvm_total = mvm_counter_ * CFG.I_BIT;
            num_mvm_sequential = mvm_counter_ * 2 * CFG.I_BIT;
            cells_per_value *= 2;
            break;
        case MappingMode::I_DIFF_W_DIFF_2XB:
            num_write = write_xbar_counter_ * 2;
            num_mvm_total = mvm_counter_ * 2 * CFG.I_BIT;
            num_mvm_sequential = mvm_counter_ * CFG.I_BIT;
            cells_per_value *= 4;
            break;
        case MappingMode::I_OFFS_W_DIFF:
            num_write = write_xbar_counter_;
            num_mvm_total = mvm_counter_ * CFG.I_BIT;
            num_mvm_sequential = mvm_counter_ * CFG.I_BIT;
            cells_per_value *= 2;
            break;
        case MappingMode::I_TC_W_DIFF:
            num_write = write_xbar_counter_;
            num_mvm_total = mvm_counter_ * CFG.I_BIT;
            num_mvm_sequential = mvm_counter_ * CFG.I_BIT;
            cells_per_value *= 2;
            break;
        case MappingMode::I_UINT_W_DIFF:
            num_write = write_xbar_counter_;
            num_mvm_total = mvm_counter_ * CFG.I_BIT;
            num_mvm_sequential = mvm_counter_ * CFG.I_BIT;
            cells_per_value *= 2;
            break;
        case MappingMode::I_UINT_W_OFFS:
            num_write = write_xbar_counter_;
            num_mvm_total = mvm_counter_ * CFG.I_BIT;
            num_mvm_sequential = mvm_counter_ * CFG.I_BIT;
            cells_per_value *= 1;
            break;
        default:
            std::cerr << "Unknown mode encountered!" << std::endl;
            std::abort();
        }

        std::cout << "num_write: " << num_write << std::endl;
        std::cout << "num_mvm_total: " << num_mvm_total << std::endl;
        std::cout << "num_mvm_sequential: " << num_mvm_sequential << std::endl;
        std::cout << "cells_per_value: " << cells_per_value << std::endl;
    }
}

} // namespace nq