/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "adc/symadc.h"

namespace nq {

SymADC::SymADC(const float min_adc_curr, const float max_adc_curr,
               const float alpha, const uint32_t resolution) :
    ADC(min_adc_curr, max_adc_curr, alpha, resolution),
    step_size_((max_adc_curr - min_adc_curr) / (std::pow(2, resolution) - 1)) {}

float SymADC::analog_digital_conversion(const float current) const {
    float clip_current = clip(current);
    float adc_res = round(clip_current / step_size_) * step_size_;
    return adc_res;
}

} // namespace nq
