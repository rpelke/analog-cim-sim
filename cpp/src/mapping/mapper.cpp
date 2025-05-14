/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "mapping/mapper.h"
#include "helper/config.h"
#include "mapping/bnn_mapper/bnn_i.h"
#include "mapping/bnn_mapper/bnn_ii.h"
#include "mapping/bnn_mapper/bnn_iii.h"
#include "mapping/bnn_mapper/bnn_iv.h"
#include "mapping/bnn_mapper/bnn_v.h"
#include "mapping/bnn_mapper/bnn_vi.h"
#include "mapping/int_mapper/int_i.h"
#include "mapping/int_mapper/int_ii.h"
#include "mapping/int_mapper/int_iii.h"
#include "mapping/int_mapper/int_iv.h"
#include "mapping/int_mapper/int_v.h"
#include "mapping/tnn_mapper/tnn_i.h"
#include "mapping/tnn_mapper/tnn_ii.h"
#include "mapping/tnn_mapper/tnn_iii.h"
#include "mapping/tnn_mapper/tnn_iv.h"
#include "mapping/tnn_mapper/tnn_v.h"

#include <algorithm>

namespace nq {

Mapper::Mapper(bool is_diff_weight_mapping) :
    is_diff_weight_mapping_(is_diff_weight_mapping),
    gd_p_(CFG.M * CFG.SPLIT.size(), std::vector<int32_t>(CFG.N, 0)),
    gd_m_(CFG.M * CFG.SPLIT.size(), std::vector<int32_t>(CFG.N, 0)),
    shift_(CFG.SPLIT.size(), 0), sum_w_(CFG.M, 0),
    ia_p_(CFG.M * CFG.SPLIT.size(), std::vector<float>(CFG.N, CFG.HRS)),
    ia_m_(CFG.M * CFG.SPLIT.size(), std::vector<float>(CFG.N, CFG.HRS)),
    i_step_size_(CFG.SPLIT.size(), 0.0),
    adc_(ADCFactory::createADC(CFG.adc_type)) {

    if (!CFG.digital_only) {
        i_mm_ = CFG.LRS - CFG.HRS;
        std::random_device hrs_rd;
        std::random_device lrs_rd;
        std::mt19937 hrs_gen(hrs_rd());
        std::mt19937 lrs_gen(lrs_rd());
        hrs_var_ = std::normal_distribution<float>(0.0f, CFG.HRS_NOISE);
        lrs_var_ = std::normal_distribution<float>(0.0f, CFG.LRS_NOISE);
    }

    if (CFG.is_int_mapping(CFG.m_mode) || (CFG.m_mode == MappingMode::TNN_IV)) {
        int curr_w_bit = CFG.W_BIT;
        for (size_t i = 0; i < CFG.SPLIT.size(); ++i) {
            shift_[i] = curr_w_bit - CFG.SPLIT[i];
            curr_w_bit -= CFG.SPLIT[i];
        }
        num_segments_ = CFG.SPLIT.size();

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
}

std::unique_ptr<Mapper> Mapper::create_from_config() {
    switch (CFG.m_mode) {
    case MappingMode::I_DIFF_W_DIFF_1XB:
        return std::make_unique<MapperIntI>();
    case MappingMode::I_DIFF_W_DIFF_2XB:
        return std::make_unique<MapperIntI>();
    case MappingMode::I_OFFS_W_DIFF:
        return std::make_unique<MapperIntII>();
    case MappingMode::I_TC_W_DIFF:
        return std::make_unique<MapperIntIII>();
    case MappingMode::I_UINT_W_DIFF:
        return std::make_unique<MapperIntIV>();
    case MappingMode::I_UINT_W_OFFS:
        return std::make_unique<MapperIntV>();
    case MappingMode::BNN_I:
        return std::make_unique<MapperBnnI>();
    case MappingMode::BNN_II:
        return std::make_unique<MapperBnnII>();
    case MappingMode::BNN_III:
        return std::make_unique<MapperBnnIII>();
    case MappingMode::BNN_IV:
        return std::make_unique<MapperBnnIV>();
    case MappingMode::BNN_V:
        return std::make_unique<MapperBnnV>();
    case MappingMode::BNN_VI:
        return std::make_unique<MapperBnnVI>();
    case MappingMode::TNN_I:
        return std::make_unique<MapperTnnI>();
    case MappingMode::TNN_II:
        return std::make_unique<MapperTnnII>();
    case MappingMode::TNN_III:
        return std::make_unique<MapperTnnIII>();
    case MappingMode::TNN_IV:
        return std::make_unique<MapperTnnIV>();
    case MappingMode::TNN_V:
        return std::make_unique<MapperTnnV>();
    default:
        std::cerr << "Mapper not implemented.";
        abort();
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

void Mapper::d_write_diff_bnn(const int32_t *mat, int32_t m_matrix,
                              int32_t n_matrix) {
    for (size_t m = 0; m < m_matrix; ++m) {
        int32_t sum_n = 0;
        for (size_t n = 0; n < n_matrix; ++n) {
            int mat_val = mat[n_matrix * m + n];
            sum_n += mat_val;
            if (mat_val == +1) {
                gd_p_[m][n] = mat_val;
                gd_m_[m][n] = 0;
            } else if (mat_val == -1) {
                gd_p_[m][n] = 0;
                gd_m_[m][n] = -mat_val;
            } else {
                std::cerr << "BNN weigth is neither +1 nor -1.";
                abort();
            }
        }
        sum_w_[m] = sum_n;
    }
}

void Mapper::d_write_diff_tnn(const int32_t *mat, int32_t m_matrix,
                              int32_t n_matrix) {
    for (size_t m = 0; m < m_matrix; ++m) {
        int32_t sum_n = 0;
        for (size_t n = 0; n < n_matrix; ++n) {
            int mat_val = mat[n_matrix * m + n];
            sum_n += mat_val;
            if (mat_val == +1) {
                gd_p_[m][n] = mat_val;
                gd_m_[m][n] = 0;
            } else if (mat_val == -1) {
                gd_p_[m][n] = 0;
                gd_m_[m][n] = -mat_val;
            } else if (mat_val == 0) {
                gd_p_[m][n] = 0;
                gd_m_[m][n] = 0;
            } else {
                std::cerr << "TNN weigth is neither 0 nor +1 nor -1";
                abort();
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

void Mapper::d_write_tc_tnn(const int32_t *mat, int32_t m_matrix,
                            int32_t n_matrix, bool offset) {
    // gd_p_ is used for bit zero (two's complement)
    // gd_m_ is used for bit one (two's complement)
    uint32_t mask_0 = 0b01;
    uint32_t mask_1 = 0b10;
    if (CFG.SPLIT == std::vector<uint32_t>{1, 1}) {
        if (offset) {
            for (size_t m = 0; m < m_matrix; ++m) {
                for (size_t n = 0; n < n_matrix; ++n) {
                    gd_p_[m][n] = (mat[n_matrix * m + n] + 1) & mask_0;
                    gd_m_[m][n] = ((mat[n_matrix * m + n] + 1) & mask_1) >> 1;
                }
            }
        } else {
            for (size_t m = 0; m < m_matrix; ++m) {
                for (size_t n = 0; n < n_matrix; ++n) {
                    gd_p_[m][n] = mat[n_matrix * m + n] & mask_0;
                    gd_m_[m][n] = (mat[n_matrix * m + n] & mask_1) >> 1;
                }
            }
        }
    } else {
        std::cerr << "Not implemented: SPLIT must be {1, 1} for TNN_IV."
                  << std::endl;
        std::exit(EXIT_FAILURE);
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

void Mapper::a_write_p_m_bnn_tnn(int32_t m_matrix, int32_t n_matrix) {
    float hrs = CFG.HRS;
    float step = CFG.LRS - hrs;
    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            ia_p_[m][n] = add_gaussian_noise(gd_p_[m][n] * step + hrs);
            ia_m_[m][n] = add_gaussian_noise(gd_m_[m][n] * step + hrs);
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

void Mapper::a_write_p_bnn(int32_t m_matrix, int32_t n_matrix) {
    float hrs = CFG.HRS;
    float step = CFG.LRS - hrs;
    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            ia_p_[m][n] = add_gaussian_noise(gd_p_[m][n] * step + hrs);
        }
    }
}

// Add Gaussian noise to a given state (current in uA).
// Gaussian noise has mean of 0 and standard deviation of stddev
// Current cannot be negative.
// For BNN and TNN only
float Mapper::add_gaussian_noise(float state) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    if (state == CFG.HRS) {
        return std::max(state + hrs_var_(gen), 0.0f);
    } else if (state == CFG.LRS) {
        return std::max(state + lrs_var_(gen), 0.0f);
    } else {
        std::cerr << "Unexpected crossbar state: " << state << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

const std::vector<std::vector<int32_t>> &Mapper::get_gd_p() const {
    return gd_p_;
}

const std::vector<std::vector<int32_t>> &Mapper::get_gd_m() const {
    return gd_m_;
}

const std::vector<std::vector<float>> &Mapper::get_ia_p() const {
    return ia_p_;
}

const std::vector<std::vector<float>> &Mapper::get_ia_m() const {
    return ia_m_;
}

void Mapper::rd_update_conductance(std::shared_ptr<const ReadDisturb> rd_model,
                                   const uint64_t read_num) {
    // Update ia_p_
    const std::vector<std::vector<uint64_t>> &cycles_p =
        rd_model->get_cycles_p();

    for (size_t i = 0; i < cycles_p.size(); i++) {
        for (size_t j = 0; j < cycles_p[i].size(); j++) {
            if (gd_p_[i][j] == 1) {
                // Update the conductance value of LRS only
                float LRS_scaling_factor =
                    rd_model->calc_G0_scaling_factor(read_num, cycles_p[i][j]);
                ia_p_[i][j] = CFG.LRS * LRS_scaling_factor;
            }
        }
    }

    if (!is_diff_weight_mapping_) {
        // No need to update ia_m_ for non-diff weight mapping
        return;
    }

    // Update ia_m_ as well
    const std::vector<std::vector<uint64_t>> &cycles_m =
        rd_model->get_cycles_m();
    for (size_t i = 0; i < cycles_m.size(); i++) {
        for (size_t j = 0; j < cycles_m[i].size(); j++) {
            if (gd_m_[i][j] == 1) {
                // Update the conductance value of LRS only
                float LRS_scaling_factor =
                    rd_model->calc_G0_scaling_factor(read_num, cycles_m[i][j]);
                ia_m_[i][j] = CFG.LRS * LRS_scaling_factor;
            }
        }
    }
}

// Check if the cells require a refresh due to the read disturb effect
// Determined analytically (i.e., no cells are measured)
bool Mapper::rd_check_refresh(std::shared_ptr<const ReadDisturb> rd_model,
                              const uint64_t read_num,
                              const uint64_t write_num) {
    float tt = rd_model->calc_transition_time(write_num);
    float t_stress = read_num * CFG.t_read;
    if (t_stress >= CFG.read_disturb_mitigation_fp * tt) {
        return true;
    }
    return false;
}

} // namespace nq
