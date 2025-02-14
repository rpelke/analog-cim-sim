/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef SYMADC_H
#define SYMADC_H

#include <cstdint>
#include <cmath>
#include <limits>

#include "adc/adc.h"

namespace nq {

class SymADC : public ADC {
    public :
        explicit SymADC(const float min_adc_curr, const float max_adc_curr, const float alpha, const uint32_t resolution);
        SymADC() = delete;
        SymADC(const SymADC&) = delete;
        virtual ~SymADC() = default;

        float analog_digital_conversion(const float current) const override;
    
    private :
        const float step_size_; // ADC step size (delta)
};

} // end namespace

#endif
