/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef ADC_H
#define ADC_H

#include <cstdint>
#include <cmath>
#include <algorithm>
#include <limits>

namespace nq {

class ADC {
    public :
        explicit ADC(const float min_adc_curr, const float max_adc_curr, const float alpha, const uint32_t resolution) :
            min_adc_curr_(min_adc_curr),
            max_adc_curr_(max_adc_curr),
            alpha_(alpha),
            resolution_(resolution) {};
        ADC() = delete;
        ADC(const ADC&) = delete;
        virtual ~ADC() = default;

        virtual float analog_digital_conversion(const float current) const = 0;
        float clip(const float current) const {
            return std::min(std::max(current, alpha_ * min_adc_curr_), alpha_ * max_adc_curr_);
        }

    protected :
        const float min_adc_curr_; // minimum current that can be converted (in uA)
        const float max_adc_curr_; // maximum current that can be converted (in uA)
        const float alpha_; // alpha value for clipping
        const uint32_t resolution_; // resolution of the ADC in bit
};

} // end namespace

#endif
