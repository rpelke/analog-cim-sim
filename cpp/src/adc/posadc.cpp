/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan                   *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "adc/posadc.h"
#include "helper/config.h"
#include <algorithm>
#include <cmath>
#include <execution>

namespace nq {

PosADC::PosADC() :
    ADC(get_min_curr(), get_max_curr()),
    step_size_((max_adc_curr_ * CFG.alpha) /
               ((std::pow(2, CFG.resolution)) - 1)) {}

float PosADC::get_max_curr() const { return CFG.N * CFG.LRS; }

float PosADC::get_min_curr() const { return 0.0; }

float PosADC::analog_digital_conversion(const float current) const {
    float clip_current = clip(current);
    float adc_res = round(clip_current / step_size_) * step_size_;
    return adc_res;
}

ADCUnsigned::ADCUnsigned() : BaseADC() { calibrate_currents(); }

void ADCUnsigned::convert(const std::vector<float> &in, std::vector<float> &out,
                          float scale, float offset) {
    // Resize output vector
    out.resize(in.size(), 0.0);

    // Add offset to input currents
    std::transform(std::execution::par, in.begin(), in.end(), out.begin(),
                   [offset](float current) { return current - offset; });

    // Clip currents
    clip(out, out);

    // Quantization
    // Calculate step size. Number of steps is reduced by one to account for
    // extra step near zero.
    float step_size = curr_range_ / (steps_ - 1);
    // Quantize and scale output
    std::transform(std::execution::par, out.begin(), out.end(), out.begin(),
                   [this, step_size, scale](float current) {
                       // Offset currents from minimum to zero to ensure
                       // rounding correctness.
                       current -= min_curr_;
                       // Divide by step size and round to quantize.
                       current = round(current / step_size);
                       // Bring value back to original level by multipliying
                       // with step size and shitfting back to minimum.
                       current = current * step_size + min_curr_;
                       // Scale and round final outputs
                       return round(current * scale);
                   });
}

float ADCUnsigned::maximum_max_current() {
    return static_cast<float>(CFG.N) * CFG.LRS;
}

float ADCUnsigned::maximum_min_current() { return 0.0; }
} // namespace nq
