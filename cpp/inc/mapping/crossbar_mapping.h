/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef CROSSBARMAPPING_H
#define CROSSBARMAPPING_H

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "helper/definitions.h"
#include "xbar/analog_crossbar.h"
#include "xbar/digital_crossbar.h"

namespace nq {

class CrossbarMapping {
  public:
    explicit CrossbarMapping(uint32_t M, uint32_t N, uint32_t W_BIT,
                             uint32_t I_BIT, std::vector<uint32_t> SPLIT,
                             const bool digital_only,
                             const INT8MappingMode m_mode, const float HRS,
                             const float LRS, const ADCType adc_type,
                             const float min_adc_curr, const float max_adc_curr,
                             const float alpha, const uint32_t resolution,
                             const bool verbose);
    CrossbarMapping() = delete;
    CrossbarMapping(const CrossbarMapping &) = delete;
    virtual ~CrossbarMapping();

    void write(const int32_t *mat, int32_t m_matrix, int32_t n_matrix);
    void mvm(int32_t *res, const int32_t *vec, const int32_t *mat,
             int32_t m_matrix, int32_t n_matrix);

  private:
    DigitalCrossbar d_xbar_;
    AnalogCrossbar a_xbar_;
    const std::vector<uint32_t> split_;
    const uint32_t i_bit_;
    const bool digital_only_;
    const INT8MappingMode m_mode_;
    uint64_t write_xbar_counter_; // Number of write function calls
    uint64_t mvm_counter_;        // Number of MVM function calls
    const bool verbose_;
};

} // namespace nq

#endif
