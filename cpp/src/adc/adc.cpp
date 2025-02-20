/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "adc/adc.h"
#include "helper/config.h"
#include <algorithm>

namespace nq {

ADC::ADC(const float min_curr, const float max_curr) :
    min_adc_curr_(min_curr), max_adc_curr_(max_curr) {}

float ADC::clip(const float current) const {
    return std::min(std::max(current, CFG.alpha * min_adc_curr_),
                    CFG.alpha * max_adc_curr_);
}

} // namespace nq
