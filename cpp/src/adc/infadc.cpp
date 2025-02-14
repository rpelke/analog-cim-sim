/******************************************************************************
 * Copyright (C) 2025 Rebecca Pelke                                           *
 * All Rights Reserved                                                        *
 *                                                                            *
 * This is work is licensed under the terms described in the LICENSE file     *
 * found in the root directory of this source tree.                           *
 ******************************************************************************/
#include "adc/infadc.h"

namespace nq {

InfADC::InfADC() :
    ADC(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max(),
        1.0, std::numeric_limits<uint32_t>::max()) {}

float InfADC::analog_digital_conversion(const float current) const {
    return current;
}

} // namespace nq
