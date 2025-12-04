/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "adc/infadc.h"
#include <algorithm>
#include <execution>

namespace nq {

InfADC::InfADC() :
    ADC(std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::max()) {}

float InfADC::analog_digital_conversion(const float current) const {
    return current;
}

ADCInfinite::ADCInfinite() : BaseADC() { calibrate_currents(); }

void ADCInfinite::convert(const std::vector<float> &in, std::vector<float> &out,
                          float scale, float offset) {
    // Resize output vector
    out.resize(in.size(), 0.0);

    // Apply offset and scale
    std::transform(
        std::execution::par, in.begin(), in.end(), out.begin(),
        [scale, offset](float current) { return (current - offset) * scale; });
}

float ADCInfinite::maximum_max_current() {
    return std::numeric_limits<float>::max();
}

float ADCInfinite::maximum_min_current() {
    return std::numeric_limits<float>::lowest();
}

} // namespace nq
