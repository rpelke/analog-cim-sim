/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan, Joel Klein       *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This work is licensed under the terms described in the LICENSE file        *
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
#include <execution>
#include <iostream>
#include <map>

namespace nq {

struct MapperRegistry {
    MapperRegistry() = delete;

    /** Everything the registry knows about one mapping mode. */
    struct MapperRegistryEntry {
        const MappingProperties *props; /**< How the mapping places a weight */
        std::unique_ptr<Mapper> (
            *create)(); /**< Builds the mapper of that mode */
    };

    template <typename T> static std::unique_ptr<Mapper> make_mapper() {
        return std::make_unique<T>();
    }

    // One row per mode, in the order of the enums.
    static constexpr MapperRegistryEntry _registry[] = {
        {&MapperIntI::PROPERTIES_1XB, &make_mapper<MapperIntI>},
        {&MapperIntI::PROPERTIES_2XB, &make_mapper<MapperIntI>},
        {&MapperIntII::PROPERTIES, &make_mapper<MapperIntII>},
        {&MapperIntIII::PROPERTIES, &make_mapper<MapperIntIII>},
        {&MapperIntIV::PROPERTIES, &make_mapper<MapperIntIV>},
        {&MapperIntV::PROPERTIES, &make_mapper<MapperIntV>},
        {&MapperBnnI::PROPERTIES, &make_mapper<MapperBnnI>},
        {&MapperBnnII::PROPERTIES, &make_mapper<MapperBnnII>},
        {&MapperBnnIII::PROPERTIES, &make_mapper<MapperBnnIII>},
        {&MapperBnnIV::PROPERTIES, &make_mapper<MapperBnnIV>},
        {&MapperBnnV::PROPERTIES, &make_mapper<MapperBnnV>},
        {&MapperBnnVI::PROPERTIES, &make_mapper<MapperBnnVI>},
        {&MapperTnnI::PROPERTIES, &make_mapper<MapperTnnI>},
        {&MapperTnnII::PROPERTIES, &make_mapper<MapperTnnII>},
        {&MapperTnnIII::PROPERTIES, &make_mapper<MapperTnnIII>},
        {&MapperTnnIV::PROPERTIES, &make_mapper<MapperTnnIV>},
        {&MapperTnnV::PROPERTIES, &make_mapper<MapperTnnV>},
    };

    /** Whether every row sits at the index of the mode it describes. */
    static constexpr bool registry_is_ordered() {
        for (size_t i = 0; i < std::size(_registry); ++i) {
            if (_registry[i].props->mode != static_cast<MappingMode>(i)) {
                return false;
            }
        }
        return true;
    }

    /** Row of a mode, aborts on NUM_MODES and on anything cast in from outside
     * the enum. */
    static const MapperRegistryEntry &entry_of(MappingMode mode,
                                               const char *what) {
        const size_t index = static_cast<size_t>(mode);
        if (index >= std::size(_registry)) {
            std::cerr << "No " << what << " for mapping mode " << index << "."
                      << std::endl;
            std::exit(EXIT_FAILURE);
        }
        return _registry[index];
    }

    static std::optional<MappingMode> mode_from_name(const std::string &name) {
        for (const MapperRegistryEntry &entry : _registry) {
            if (name == entry.props->name) {
                return entry.props->mode;
            }
        }
        return {};
    }

    static std::string name_from_mode(MappingMode mode) {
        const size_t index = static_cast<size_t>(mode);
        if (index >= std::size(_registry)) {
            return "Unknown mode";
        }
        return _registry[index].props->name;
    }
};

static_assert(MapperRegistry::registry_is_ordered(),
              "The mapping registry rows are not in MappingMode order");

Mapper::Mapper(const MappingProperties &props) :
    props_(props),
    gd_p_(CFG.state_columns(), std::vector<int32_t>(CFG.capacity().n, 0)),
    gd_m_(CFG.state_columns(), std::vector<int32_t>(CFG.capacity().n, 0)),
    shift_(CFG.SPLIT.size(), 0),
    sum_w_(CFG.capacity().m, 0),
    ia_p_(CFG.state_columns(), std::vector<float>(CFG.capacity().n, CFG.HRS)),
    ia_m_(CFG.state_columns(), std::vector<float>(CFG.capacity().n, CFG.HRS)),
    ia_p_orig_(CFG.state_columns(),
               std::vector<float>(CFG.capacity().n, CFG.HRS)),
    ia_m_orig_(CFG.state_columns(),
               std::vector<float>(CFG.capacity().n, CFG.HRS)),
    i_step_size_(CFG.SPLIT.size(), 0.0),
    adc_(ADCFactory::createADC(CFG.adc_type)) {

    if (!CFG.digital_only) {
        i_mm_ = CFG.LRS - CFG.HRS;
        std::random_device hrs_rd;
        std::random_device lrs_rd;
        std::mt19937 hrs_gen(hrs_rd());
        std::mt19937 lrs_gen(lrs_rd());
        if (CFG.HRS_NOISE > 0.0f) {
            hrs_var_ = std::normal_distribution<float>(0.0f, CFG.HRS_NOISE);
        }
        if (CFG.LRS_NOISE > 0.0f) {
            lrs_var_ = std::normal_distribution<float>(0.0f, CFG.LRS_NOISE);
        }

        if (CFG.parasitics) {
            par_solver_ = std::make_shared<ParasiticSolver>(CFG.w_res,
                                                            CFG.V_read, props_);
        }
    }

    if (CFG.is_int_mapping(CFG.m_mode) || (CFG.m_mode == MappingMode::TNN_IV)) {
        int curr_w_bit = CFG.W_BIT;
        for (size_t i = 0; i < CFG.SPLIT.size(); ++i) {
            shift_[i] = curr_w_bit - CFG.SPLIT[i];
            curr_w_bit -= CFG.SPLIT[i];
        }
        num_segments_ = CFG.SPLIT.size();

        for (size_t s = 0; s < num_segments_; ++s) {
            i_step_size_[s] = i_mm_ / ((1 << CFG.SPLIT[s]) - 1);
        }
    }
}

std::unique_ptr<Mapper> Mapper::create() {
    return MapperRegistry::entry_of(CFG.m_mode, "mapper").create();
}

const MappingProperties &Mapper::properties() const { return props_; }

bool Mapper::uses_negative_matrix() const {
    return props_.uses_negative_matrix();
}

const MappingProperties &Mapper::properties(MappingMode mode) {
    return *MapperRegistry::entry_of(mode, "properties").props;
}

std::optional<MappingMode> Mapper::mode_from_name(const std::string &name) {
    return MapperRegistry::mode_from_name(name);
}

std::string Mapper::name_from_mode(MappingMode mode) {
    return MapperRegistry::name_from_mode(mode);
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
                std::cerr << "BNN weight is neither +1 nor -1.";
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
            ia_p_[m][n] =
                add_gaussian_noise(gd_p_[m][n] * step + hrs, gd_p_[m][n]);
            ia_p_orig_[m][n] = ia_p_[m][n];

            ia_m_[m][n] =
                add_gaussian_noise(gd_m_[m][n] * step + hrs, gd_m_[m][n]);
            ia_m_orig_[m][n] = ia_m_[m][n];
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
            ia_p_[m][n] =
                add_gaussian_noise(gd_p_[m][n] * step + hrs, gd_p_[m][n]);
            ia_p_orig_[m][n] = ia_p_[m][n];
        }
    }
}

// Add Gaussian noise to a given state (current in uA).
// Gaussian noise has mean of 0 and standard deviation of stddev
// Current cannot be negative.
// For BNN and TNN only
float Mapper::add_gaussian_noise(float state, int32_t mask) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    if (mask == 0) {
        if (CFG.HRS_NOISE <= 0.0f) {
            return state;
        }
        return std::max(state + hrs_var_(gen), 0.0f);
    } else if (mask == 1) {
        if (CFG.LRS_NOISE <= 0.0f) {
            return state;
        }
        return std::max(state + lrs_var_(gen), 0.0f);
    } else {
        std::cerr << "Unexpected crossbar value: " << mask << std::endl;
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

    if (!uses_negative_matrix()) {
        // No need to update ia_m_ if the mapping leaves it empty
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

void Mapper::rd_update_conductance(
    std::shared_ptr<const ReadDisturb> rd_model,
    const std::vector<std::vector<uint64_t>> &consecutive_reads_p,
    const std::vector<std::vector<uint64_t>> &consecutive_reads_m) {
    // Update ia_p_
    const std::vector<std::vector<uint64_t>> &cycles_p =
        rd_model->get_cycles_p();

    for (size_t i = 0; i < cycles_p.size(); i++) {
        for (size_t j = 0; j < cycles_p[i].size(); j++) {
            if (gd_p_[i][j] == 1) {
                // Update the conductance value of LRS only
                float LRS_scaling_factor = rd_model->calc_G0_scaling_factor(
                    consecutive_reads_p[i][j], cycles_p[i][j]);
                ia_p_[i][j] = CFG.LRS * LRS_scaling_factor;
            }
        }
    }

    if (!uses_negative_matrix()) {
        // No need to update ia_m_ if the mapping leaves it empty
        return;
    }

    // Update ia_m_ as well
    const std::vector<std::vector<uint64_t>> &cycles_m =
        rd_model->get_cycles_m();
    for (size_t i = 0; i < cycles_m.size(); i++) {
        for (size_t j = 0; j < cycles_m[i].size(); j++) {
            if (gd_m_[i][j] == 1) {
                // Update the conductance value of LRS only
                float LRS_scaling_factor = rd_model->calc_G0_scaling_factor(
                    consecutive_reads_m[i][j], cycles_m[i][j]);
                ia_m_[i][j] = CFG.LRS * LRS_scaling_factor;
            }
        }
    }
}

// SOFTWARE refresh strategy for read disturb mitigation
// Check if the cells require a refresh due to the read disturb effect
// Determined analytically (i.e., no cells are measured)
bool Mapper::rd_check_software_refresh(
    std::shared_ptr<const ReadDisturb> rd_model, const uint64_t read_num,
    const uint64_t write_num) {
    float tt = rd_model->calc_transition_time(write_num);
    float t_stress = read_num * CFG.t_read;
    if (t_stress >= CFG.read_disturb_mitigation_fp * tt) {
        return true;
    }
    return false;
}

// CELL_BASED refresh strategy for read disturb mitigation
int Mapper::rd_cell_based_refresh(std::shared_ptr<ReadDisturb> rd_model) {
    // Count refresh operations
    int refresh_count = 0;

    float tolerance = CFG.read_disturb_update_tolerance;
    float lrs = CFG.LRS;

    for (size_t m = 0; m < ia_p_.size(); ++m) {
        for (size_t n = 0; n < ia_p_[m].size(); ++n) {
            if (gd_p_[m][n] == 1) {
                // Cell [m][n] is LRS cell -> Check conductance
                if ((ia_p_[m][n] < (1 - tolerance) * lrs) ||
                    (ia_p_[m][n] > (1 + tolerance) * lrs)) {
                    // Refresh needed
                    ia_p_[m][n] = add_gaussian_noise(lrs, 1);
                    // Update cycle count
                    rd_model->update_cycle_p(m, n, 1);
                    // Reset consecutive reads
                    rd_model->reset_consecutive_reads_p(m, n);
                    // Increment refresh count
                    refresh_count++;
                }
            }
        }
    }

    if (!uses_negative_matrix()) {
        // No need to check ia_m_ if the mapping leaves it empty
        return refresh_count;
    }

    for (size_t m = 0; m < ia_m_.size(); ++m) {
        for (size_t n = 0; n < ia_m_[m].size(); ++n) {
            if (gd_m_[m][n] == 1) {
                // Cell [m][n] is LRS cell -> Check conductance
                if ((ia_m_[m][n] < (1 - tolerance) * lrs) ||
                    (ia_m_[m][n] > (1 + tolerance) * lrs)) {
                    // Refresh needed
                    ia_m_[m][n] = add_gaussian_noise(lrs, 1);
                    // Update cycle count
                    rd_model->update_cycle_m(m, n, 1);
                    // Reset consecutive reads
                    rd_model->reset_consecutive_reads_m(m, n);
                    // Increment refresh count
                    refresh_count++;
                }
            }
        }
    }
    return refresh_count;
}

void Mapper::slice_vd(std::vector<int32_t> &vd, std::vector<int32_t> &vd_slice,
                      size_t n, size_t i_bit) {
    std::transform(std::execution::par, vd.begin(), vd.begin() + n,
                   vd_slice.begin(),
                   [i_bit](int32_t v) { return (v >> i_bit) & 1; });
}

void Mapper::a_add_c2c_var(int32_t m_matrix, int32_t n_matrix) {
    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            ia_p_[m][n] = add_gaussian_noise(ia_p_orig_[m][n], gd_p_[m][n]);
            ia_m_[m][n] = add_gaussian_noise(ia_m_orig_[m][n], gd_m_[m][n]);
        }
    }
}

void Mapper::a_remove_c2c_var(int32_t m_matrix, int32_t n_matrix) {
    for (size_t m = 0; m < m_matrix; ++m) {
        for (size_t n = 0; n < n_matrix; ++n) {
            ia_p_[m][n] = ia_p_orig_[m][n];
            ia_m_[m][n] = ia_m_orig_[m][n];
        }
    }
}

} // namespace nq
