#include "xbar/adc.h"
#include "helper/config.h"

#include <algorithm>
#include <cmath>
#include <execution>

namespace nq {

ADC::ADC() :
    resolution_(CFG.resolution),
    steps_(std::pow(2, resolution_)),
    hists_(ADCHistograms::get_instance()) {}

void ADC::convert(const std::vector<float> &in, std::vector<float> &out,
                  float scale, float offset, const char *l_name) {
    if (CFG.adc_profile) {
        profile_inputs(in, l_name);
    }

    // Resize output vector
    out.resize(in.size(), 0.0);

    // Apply offset and scale
    std::transform(std::execution::par, in.begin(), in.end(), out.begin(),
                   [this, scale, offset](float current) {
                       return convert(current, scale, offset);
                   });
}

void ADC::calibrate_currents() {
    // TODO: Implement with calibration mode
    max_curr_ = maximum_max_current();
    min_curr_ = maximum_min_current();

    curr_range_ = max_curr_ - min_curr_;
}

float ADC::clip(float current) {
    return std::min(std::max(current, min_curr_), max_curr_);
}

void ADC::profile_inputs(const std::vector<float> &in, const char *l_name) {
    // Check if a histogram already exists for the given layer, otherwise
    // create one.
    if (!hists_.get().has_histogram(l_name)) {
        hists_.get().add_histogram(l_name, min_curr_, max_curr_);
    }

    // Update values in layer histogram
    std::reference_wrapper<BinnedHistogram> l_hist(
        hists_.get().get_histogram(l_name).value());
    l_hist.get().update(in);
}

ADCInfinite::ADCInfinite() : ADC() { calibrate_currents(); }

float ADCInfinite::convert(const float current, float scale, float offset) {
    return (current + offset) * scale;
}

float ADCInfinite::maximum_max_current() {
    return std::numeric_limits<float>::max();
}

float ADCInfinite::maximum_min_current() {
    return std::numeric_limits<float>::lowest();
}

ADCUnsigned::ADCUnsigned() : ADC() { calibrate_currents(); }

float ADCUnsigned::convert(const float current, float scale, float offset) {
    float tmp;
    // Add offset to input current
    tmp = current + offset;
    // Clip currents
    tmp = clip(tmp);

    // Quantization
    // Calculate step size. Number of steps is reduced by one to account for
    // extra step near zero.
    float step_size = curr_range_ / (steps_ - 1);
    // Offset current from minimum to zero to ensure
    // rounding correctness.
    tmp -= min_curr_;
    // Divide by step size and round to quantize.
    tmp = round(tmp / step_size);
    // Bring value back to original level by multipliying
    // with step size and shitfting back to minimum.
    tmp = tmp * step_size + min_curr_;

    // Scale and round final output
    return round(tmp * scale);
}

float ADCUnsigned::maximum_max_current() {
    return static_cast<float>(CFG.N) * CFG.LRS;
}

float ADCUnsigned::maximum_min_current() { return 0.0; }

ADCSigned::ADCSigned() : ADCUnsigned() {
    calibrate_currents();
    // Reduce number of steps for signed zero.
    steps_ -= 1;
}

float ADCSigned::maximum_max_current() {
    return static_cast<float>(CFG.N) * (CFG.LRS - CFG.HRS);
}

float ADCSigned::maximum_min_current() {
    return -static_cast<float>(CFG.N) * (CFG.LRS - CFG.HRS);
}

std::unique_ptr<ADC> ADCFactory::createADC(ADCType type) {
    switch (type) {
    case ADCType::INF_ADC:
        return std::make_unique<ADCInfinite>();
    case ADCType::SYM_RANGE_ADC:
        return std::make_unique<ADCSigned>();
    case ADCType::POS_RANGE_ONLY_ADC:
        return std::make_unique<ADCUnsigned>();
    default:
        return nullptr;
    }
}

} // namespace nq
