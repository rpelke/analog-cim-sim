/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#ifndef INFADC_H
#define INFADC_H

#include <cstdint>
#include <cmath>

#include "adc/adc.h"

namespace nq {

// ADC with infinite resolution
class InfADC : public ADC {
    public :
        InfADC();
        InfADC(const InfADC&) = delete;
        virtual ~InfADC() = default;

        float analog_digital_conversion(const float current) const override;
};

} // end namespace

#endif
