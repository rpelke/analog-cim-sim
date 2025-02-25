/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "mapping/mapper.h"
#include "helper/config.h"
#include "mapping/mapperA.h"
#include "mapping/mapperB.h"
#include "mapping/mapperC.h"
#include "mapping/mapperD.h"
#include "mapping/mapperE.h"

namespace nq {

Mapper::Mapper(bool is_diff_weight_mapping) :
    is_diff_weight_mapping_(is_diff_weight_mapping),
    gd_p_(CFG.M * CFG.SPLIT.size(), std::vector<int32_t>(CFG.N, 0)),
    gd_m_(CFG.M * CFG.SPLIT.size(), std::vector<int32_t>(CFG.N, 0)),
    shift_(CFG.SPLIT.size(), 0), sum_w_(CFG.M, 0),
    ia_p_(CFG.M * CFG.SPLIT.size(), std::vector<float>(CFG.N, 0)),
    ia_m_(CFG.M * CFG.SPLIT.size(), std::vector<float>(CFG.N, 0)),
    i_step_size_(CFG.SPLIT.size(), 0.0),
    adc_(ADCFactory::createADC(CFG.adc_type)) {

    int curr_w_bit = CFG.W_BIT;
    for (size_t i = 0; i < CFG.SPLIT.size(); ++i) {
        shift_[i] = curr_w_bit - CFG.SPLIT[i];
        curr_w_bit -= CFG.SPLIT[i];
    }

    if (!CFG.digital_only) {
        num_segments_ = CFG.SPLIT.size();
        i_mm_ = CFG.LRS - CFG.HRS;
    }

    if (is_diff_weight_mapping_) {
        for (size_t s = 0; s < num_segments_; ++s) {
            i_step_size_[s] = i_mm_ / ((1 << (CFG.SPLIT[s] - 1)));
        }
    } else {
        for (size_t s = 0; s < num_segments_; ++s) {
            i_step_size_[s] = i_mm_ / ((1 << CFG.SPLIT[s]) - 1);
        }
    }
}

std::unique_ptr<Mapper> Mapper::create_from_config() {
    switch (CFG.m_mode) {
    case INT8MappingMode::I_DIFF_W_DIFF_1XB:
        return std::make_unique<MapperA>();
    case INT8MappingMode::I_DIFF_W_DIFF_2XB:
        return std::make_unique<MapperA>();
    case INT8MappingMode::I_OFFS_W_DIFF:
        return std::make_unique<MapperB>();
    case INT8MappingMode::I_TC_W_DIFF:
        return std::make_unique<MapperC>();
    case INT8MappingMode::I_UINT_W_DIFF:
        return std::make_unique<MapperD>();
    case INT8MappingMode::I_UINT_W_OFFS:
        return std::make_unique<MapperE>();
    default:
        throw std::runtime_error("Mapper not implemented.");
    }
}

void Mapper::d_write_diff(const int32_t *mat, int32_t m_matrix,
                          int32_t n_matrix) {
    const std::vector<uint32_t> &split = CFG.SPLIT;
    for (size_t m = 0; m < m_matrix; ++m) {
        int32_t sum_n = 0;
        for (size_t n = 0; n < n_matrix; ++n) {
            int mat_val = mat[n_matrix * m + n];
            sum_n += mat_val;
            for (size_t s = 0; s < split.size(); ++s) {
                int gd_idx = m * split.size() + s;
                if (mat_val >= 0) {
                    gd_p_[gd_idx][n] =
                        (mat_val >> shift_[s]) & ((1 << split[s]) - 1);
                    gd_m_[gd_idx][n] = 0;
                } else {
                    gd_p_[gd_idx][n] = 0;
                    gd_m_[gd_idx][n] =
                        (-mat_val >> shift_[s]) & ((1 << split[s]) - 1);
                }
            }
        }
        sum_w_[m] = sum_n;
    }
}

void Mapper::d_write_offs(const int32_t *mat, int32_t m_matrix,
                          int32_t n_matrix) {
    const std::vector<uint32_t> &split = CFG.SPLIT;
    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            int mat_val = mat[n_matrix * m + n] + (1 << (CFG.W_BIT - 1));
            for (size_t s = 0; s < split.size(); ++s) {
                int gd_idx = m * split.size() + s;
                gd_p_[gd_idx][n] =
                    (mat_val >> shift_[s]) & ((1 << split[s]) - 1);
            }
        }
    }
}

void Mapper::a_write_p_m(int32_t m_matrix, int32_t n_matrix) {
    float hrs = CFG.HRS;
    for (size_t m = 0; m < m_matrix * num_segments_; ++m) {
        float step = i_step_size_[m % num_segments_];
        for (size_t n = 0; n < n_matrix; ++n) {
            ia_p_[m][n] = gd_p_[m][n] * step + hrs;
            ia_m_[m][n] = gd_m_[m][n] * step + hrs;
        }
    }
}

void Mapper::a_write_p(int32_t m_matrix, int32_t n_matrix) {
    float hrs = CFG.HRS;
    for (size_t m = 0; m < m_matrix * num_segments_; ++m) {
        float step = i_step_size_[m % num_segments_];
        for (size_t n = 0; n < n_matrix; ++n) {
            ia_p_[m][n] = gd_p_[m][n] * step + hrs;
        }
    }
}

} // namespace nq