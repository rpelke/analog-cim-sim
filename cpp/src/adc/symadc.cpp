/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "adc/symadc.h"
#include "helper/config.h"
#include <cmath>

namespace nq {

SymADC::SymADC() :
    ADC(get_min_curr(), get_max_curr()),
    step_size_((2 * max_adc_curr_ * CFG.alpha) /
               ((std::pow(2, CFG.resolution)) - 1)) {}

float SymADC::get_max_curr() const { return CFG.N * (CFG.LRS - CFG.HRS); }

float SymADC::get_min_curr() const {
    return -static_cast<float>(CFG.N) * (CFG.LRS - CFG.HRS);
}

// Mid-tread quantization
float SymADC::analog_digital_conversion(const float current) const {
    float clip_current = clip(current);
    float adc_res = round(clip_current / step_size_) * step_size_;
    return adc_res;
}

} // namespace nq
