/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke, Arunkumar Vaidyanathan                   *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This work is licensed under the terms described in the LICENSE file        *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/

#include "xbar/adc.h"
#include "helper/config.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <execution>
#include <iostream>

namespace nq {

ADC::ADC() :
    resolution_(CFG.resolution),
    steps_(std::pow(2, resolution_)),
    hists_(ADCHistograms::get_instance()) {}

void ADC::convert(const std::vector<float> &in, std::vector<float> &out,
                  const int32_t len, float scale, float offset,
                  const char *l_name) {
    // Check if len is less than input vector length.
    if (in.size() < len) {
        std::cerr << "Requested ADC conversion length: " << len
                  << ", is greater than input "
                  << "vector length: " << in.size() << "!" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    if (CFG.adc_profile) {
        profile_inputs(in, len, l_name);
    }

    // Resize output vector
    if (out.size() < len) {
        out.resize(len, 0.0);
    }

    // Apply offset and scale
    std::transform(std::execution::unseq, in.begin(), in.begin() + len,
                   out.begin(), [this, scale, offset, l_name](float current) {
                       return convert(current, scale, offset, l_name);
                   });
}

std::pair<float, float> ADC::get_currents(const char *l_name) {
    switch (CFG.adc_calib_mode) {
    case ADCCalibMode::MAX:
        return std::make_pair<float, float>(maximum_min_current(),
                                            maximum_max_current());
    case ADCCalibMode::CALIB:
        if (auto val = CFG.adc_calib_dict.find(l_name);
            val != CFG.adc_calib_dict.end()) {
            return val->second;
        } else {
            std::cerr << "Unable to find calibrated ADC currents for layer: "
                      << l_name << std::endl;
            std::exit(EXIT_FAILURE);
        }
    default:
        std::cerr << "Unknown ADC calibration mode!" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

float ADC::clip(float current, float min_curr, float max_curr) {
    return std::min(std::max(current, min_curr), max_curr);
}

void ADC::profile_inputs(const std::vector<float> &in, const int32_t len,
                         const char *l_name) {
    // Check if a histogram already exists for the given layer, otherwise
    // create one.
    if (!hists_.get().has_histogram(l_name)) {
        hists_.get().add_histogram(l_name, maximum_min_current(),
                                   maximum_max_current(),
                                   CFG.adc_profile_bin_size);
    }

    // Update values in layer histogram
    std::reference_wrapper<BinnedHistogram> l_hist(
        hists_.get().get_histogram(l_name).value());
    l_hist.get().update(std::vector<float>(in.begin(), in.begin() + len));
}

ADCInfinite::ADCInfinite() : ADC() {}

float ADCInfinite::convert(const float current, double scale, float offset,
                           const char *l_name, float level) {
    return (current + offset) * static_cast<float>(scale);
}

float ADCInfinite::maximum_max_current() {
    return std::numeric_limits<float>::max();
}

float ADCInfinite::maximum_min_current() {
    return std::numeric_limits<float>::lowest();
}

ADCUnsigned::ADCUnsigned() : ADC() {}

float ADCUnsigned::convert(const float current, double scale, float offset,
                           const char *l_name, float level) {
    float tmp;
    // Get current ranges
    std::pair<float, float> currs = get_currents(l_name);
    float min_curr = currs.first;
    float max_curr = currs.second;
    float curr_range = max_curr - min_curr;
    // Add offset to input current
    tmp = current + offset;
    // Clip currents
    tmp = clip(tmp, min_curr, max_curr);

    // Quantization
    // Calculate step size. Number of steps is reduced by one to account for
    // extra step near zero.
    float step_size = curr_range / (steps_ - 1);
    // Offset current from minimum to zero to ensure
    // rounding correctness.
    tmp -= min_curr;
    // Divide by step size and round to quantize.
    tmp = round(tmp / step_size);
    // Bring value back to original level by multipliying
    // with step size and shitfting back to minimum.
    tmp = tmp * step_size + min_curr;

    // Scale and round final output
    return round(tmp * static_cast<float>(scale));
}

float ADCUnsigned::maximum_max_current() {
    return static_cast<float>(CFG.capacity().n) * CFG.LRS;
}

float ADCUnsigned::maximum_min_current() { return 0.0; }

ADCSigned::ADCSigned() : ADCUnsigned() {
    // Reduce number of steps for signed zero.
    steps_ -= 1;
}

float ADCSigned::maximum_max_current() {
    return static_cast<float>(CFG.capacity().n) * (CFG.LRS - CFG.HRS);
}

float ADCSigned::maximum_min_current() {
    return -static_cast<float>(CFG.capacity().n) * (CFG.LRS - CFG.HRS);
}

ADCSignedLevel::ADCSignedLevel() : ADCSigned() {}

float ADCSignedLevel::convert(const float current, double scale, float offset,
                              const char *l_name, float level) {
    if (level <= 0.0) {
        return ADCSigned::convert(current, scale, offset, l_name);
    }
    // Get current ranges
    std::pair<float, float> currs = get_currents(l_name);
    double step = step_size(currs.second - currs.first, level);

    // A current of zero gives code 0, so every whole number of levels lies on
    // a code. Only the codes within the current range exist.
    constexpr double eps = 1e-9;
    int64_t min_code =
        static_cast<int64_t>(std::ceil(currs.first / step - eps));
    int64_t max_code =
        static_cast<int64_t>(std::floor(currs.second / step + eps));
    if (min_code > max_code) {
        min_code = max_code = std::llround(
            (static_cast<double>(currs.first) + currs.second) / (2.0 * step));
    }

    // Quantization with clipping
    int64_t code = std::clamp<int64_t>(std::llround((current + offset) / step),
                                       min_code, max_code);

    // Scale and round final output
    return static_cast<float>(std::round(code * step * scale));
}

double ADCSignedLevel::step_size(double curr_range, float level) const {
    constexpr double eps = 1e-9;
    // Step size of an ADCSigned with the same range
    double step = curr_range / (steps_ - 1);
    // Enough resolution: the finest whole fraction of a level. Otherwise the
    // smallest whole number of levels, so that the range stays covered.
    if (step <= level * (1.0 + eps)) {
        return level / std::floor(level / step + eps);
    }
    return level * std::ceil(step / level - eps);
}

std::unique_ptr<ADC> ADCFactory::createADC(ADCType type) {
    switch (type) {
    case ADCType::INF_ADC:
        return std::make_unique<ADCInfinite>();
    case ADCType::SYM_RANGE_ADC:
        return std::make_unique<ADCSigned>();
    case ADCType::SYM_LEVEL_ADC:
        return std::make_unique<ADCSignedLevel>();
    case ADCType::POS_RANGE_ONLY_ADC:
        return std::make_unique<ADCUnsigned>();
    default:
        return nullptr;
    }
}

} // namespace nq
