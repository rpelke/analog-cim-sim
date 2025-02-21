/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "adc/posadc.h"
#include "helper/config.h"
#include <cmath>

namespace nq {

PosADC::PosADC() :
    ADC(get_min_curr(), get_max_curr()),
    step_size_((max_adc_curr_ * CFG.alpha) /
               ((std::pow(2, CFG.resolution)) - 1)) {}

float PosADC::get_max_curr() const {
    if (CFG.m_mode == INT8MappingMode::I_UINT_W_OFFS) {
        return CFG.N * CFG.LRS;
    } else {
        throw std::runtime_error(
            "PosADC should be used with I_UINT_W_OFFS mapping.");
    }
}

float PosADC::get_min_curr() const { return 0.0; }

float PosADC::analog_digital_conversion(const float current) const {
    float clip_current = clip(current);
    float adc_res = round(clip_current / step_size_) * step_size_;
    return adc_res;
}

} // namespace nq
