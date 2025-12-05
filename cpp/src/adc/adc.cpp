/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan                   *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "adc/adc.h"
#include "helper/config.h"
#include <algorithm>
#include <cmath>
#include <execution>

namespace nq {

ADC::ADC(const float min_curr, const float max_curr) :
    min_adc_curr_(min_curr),
    max_adc_curr_(max_curr) {}

float ADC::clip(const float current) const {
    return std::min(std::max(current, CFG.alpha * min_adc_curr_),
                    CFG.alpha * max_adc_curr_);
}

BaseADC::BaseADC() :
    resolution_(CFG.resolution),
    steps_(std::pow(2, resolution_)) {}

void BaseADC::calibrate_currents() {
    // TODO: Implement with calibration mode
    max_curr_ = maximum_max_current();
    min_curr_ = maximum_min_current();

    curr_range_ = max_curr_ - min_curr_;
}

float BaseADC::clip(float current) {
    return std::min(std::max(current, min_curr_), max_curr_);
}

} // namespace nq
